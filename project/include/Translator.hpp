#ifndef PROJECT_INCLUDE_TRANSLATOR_HPP_
#define PROJECT_INCLUDE_TRANSLATOR_HPP_

#include <exception>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

namespace generator {
    namespace exceptions {
        class TranslatorError : public std::exception {
         public:
            const char *what() const noexcept override { return "TranslatorError occur"; }
        };

        class InvalidStreamError : public TranslatorError {
         public:
            const char *what() const noexcept override {
                return "InvalidStreamError occur. Translator got invalid stream.";
            }
        };

        class GemtextFormatError : public TranslatorError {
         public:
            const char *what() const noexcept override { return "GemtextFormatError occur."; }
        };

        class HeaderFormatError : public GemtextFormatError {
         public:
            const char *what() const noexcept override { return "HeaderFormatError occur."; }
        };

        class BlockquoteFormatError : public GemtextFormatError {
         public:
            const char *what() const noexcept override { return "BlockquoteFormatError occur."; }
        };

        class LinkFormatError : public GemtextFormatError {
         public:
            const char *what() const noexcept override { return "LinkFormatError occur."; }
        };

        class PreformedFormatError : public GemtextFormatError {
         public:
            const char *what() const noexcept override { return "PreformedFormatError occur."; }
        };

        class ListFormatError : public GemtextFormatError {
         public:
            const char *what() const noexcept override { return "ListFormatError occur."; }
        };
    }  // namespace exceptions

    /**
     * This class implements the translator interface. A translator is a class that is
     * capable of transferring input data from a stream in one format to an output
     * stream in another format.
     */
    class BasicTranslator {
     public:
        using TranslatorShPtr = std::shared_ptr<BasicTranslator>;
        using TranslatorWkPtr = std::shared_ptr<BasicTranslator>;
        using IStreamType = std::istream;
        using OStreamType = std::ostream;

        BasicTranslator() = default;

        /**
         * Virtual function, that is specific for different types of translators.
         * THis function implement translation routine.
         * @param is Input stream.
         * @param os Output stream.
         */
        virtual void Translate(IStreamType &is, OStreamType &os) = 0;

        virtual ~BasicTranslator() = default;
    };

    /**
     * DefaultTranslator just trivially copy data from one input stream
     * to output stream.
     */
    class DefaultTranslator : public BasicTranslator {
     public:
        void Translate(IStreamType &is, OStreamType &os) override;
    };

    class GemToHTMLTranslator : public BasicTranslator {
     public:
        void Translate(IStreamType &is, OStreamType &os) override;

     private:
        using LineType = std::string;

        // Frequently used tags and representative tag constants.
        static constexpr std::string_view BLANK_LINE = "<br/>";
        static constexpr std::string_view paragraph_open = "<p>";
        static constexpr std::string_view paragraph_close = "</p>";

        // gemtext lines possible prefixes
        static constexpr std::string_view LINK_PREFIX = "=>";
        static constexpr std::string_view HEADING_PREFIX = "#";
        static constexpr std::string_view LIST_PREFIX = "*";
        static constexpr std::string_view BLOCKQUOTE_PREFIX = ">";
        static constexpr std::string_view PREFORMED_PREFIX = "```";

        /**
         * Translates input strings from gemtext to html. In this case, the parser
         * needs to remember whether it is in the state of reading preformatted data.
         * @param line Gemtext line.
         * @param preformed_state Current state of preformed text reading.
         * @return HTML line.
         */
        LineType TranslateLine(const LineType &line, bool &preformed_state) const;

        void WriteHeader(OStreamType &os) const;
        void WriteFooter(OStreamType &os) const;

        /*
         * Bellow functions translate certain types of input gemtext lines.
         */
        LineType ParagraphTranslator(const LineType &line) const;
        LineType BlockquoteTranslator(const LineType &line) const;
        LineType HeaderTranslator(const LineType &line) const;
        LineType LinkTranslator(const LineType &line) const;
        LineType ListTranslator(const LineType &line) const;
    };

    /**
     * Creates a pointer to a polymorphic translator class.
     * @tparam TranslatorType Translator type.
     * @tparam Args Translator specific constructor arguments types.
     * @param args Constructor arguments.
     * @return TranslatorShPtr, i. e. pointer to translator.
     */
    template <typename TranslatorType, typename... Args>
    BasicTranslator::TranslatorShPtr CreateTranslator(Args &&...args) {
        return std::make_unique<TranslatorType>(std::forward<Args>(args)...);
    }
}  // namespace generator

#endif  // PROJECT_INCLUDE_TRANSLATOR_HPP_
