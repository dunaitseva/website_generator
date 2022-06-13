#include "Translator.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>

namespace generator {
    namespace {
        constexpr char WS = 32;

        template <typename PrefixType>
        bool LineStartWith(const std::string &string, const PrefixType &prefix) {
            constexpr size_t valid_pos = 0;
            return string.rfind(prefix, 0) == valid_pos;
            // return string.starts_with(prefix);
        }

        // SkipLeadingWs should work fast enough, because it
        // don't call copy constructor and using RVO.
        std::string SkipLeadingWs(const std::string &line) {
            size_t i = 0;
            // clang-format off
            for (; i < line.size() && line[i] == WS; ++i) {}
            // clang-format on
            return {line.begin() + i, line.end()};
        }

    }  // namespace

    void DefaultTranslator::Translate(IStreamType &is, OStreamType &os) {
        std::noskipws(is);
        std::istream_iterator<char> it_in{is};
        std::istream_iterator<char> it_eof;
        std::ostream_iterator<char> it_out{os};

        std::copy(it_in, it_eof, it_out);
    }

    void GemToHTMLTranslator::Translate(IStreamType &is, OStreamType &os) {
        WriteHeader(os);
        bool preformed_state = false;
        bool is_list = false;

        auto ListControl = [&is_list](const LineType &line) -> LineType {
            if (LineStartWith(line, LIST_PREFIX) && !is_list) {
                is_list = !is_list;
                return "<ul>\n";
            } else if (!LineStartWith(line, LIST_PREFIX) && is_list) {
                is_list = !is_list;
                return "</ul>\n";
            } else {
                return {};
            }
        };

        while (!is.eof()) {
            LineType line;
            std::getline(is, line);
            LineType translated_line = TranslateLine(line, preformed_state);
            os << ListControl(line) << translated_line << '\n';
        }
        os << ListControl("");

        if (preformed_state) {
            throw exceptions::PreformedFormatError();
        }

        WriteFooter(os);
    }

    GemToHTMLTranslator::LineType GemToHTMLTranslator::TranslateLine(const LineType &line,
                                                                     bool &preformed_state) const {
        // String translation, depends on line prefix
        if (LineStartWith(line, PREFORMED_PREFIX)) {
            preformed_state = !preformed_state;
            return {};
        }

        if (preformed_state) {
            return line;
        }

        if (LineStartWith(line, LINK_PREFIX)) {
            return LinkTranslator(line);
        }

        if (LineStartWith(line, HEADING_PREFIX)) {
            return HeaderTranslator(line);
        }

        if (LineStartWith(line, LIST_PREFIX)) {
            return ListTranslator(line);
        }

        if (LineStartWith(line, BLOCKQUOTE_PREFIX)) {
            return BlockquoteTranslator(line);
        }

        if (line.empty()) {
            return BLANK_LINE.data();
        }

        return ParagraphTranslator(line);
    }

    GemToHTMLTranslator::LineType GemToHTMLTranslator::ParagraphTranslator(const LineType &line) const {
        return paragraph_open.data() + line + paragraph_close.data();
    }

    GemToHTMLTranslator::LineType GemToHTMLTranslator::BlockquoteTranslator(const LineType &line) const {
        static const LineType blockquote_open = "<blockquote>";
        static const LineType blockquote_close = "</blockquote>";
        LineType content = SkipLeadingWs({line.begin() + BLOCKQUOTE_PREFIX.size(), line.end()});
        if (content.empty()) {
            throw exceptions::BlockquoteFormatError();
        }

        return blockquote_open + paragraph_open.data() + content + paragraph_close.data() + blockquote_close;
    }

    GemToHTMLTranslator::LineType GemToHTMLTranslator::HeaderTranslator(const LineType &line) const {
        // Bellow is three types of headers, that gemtext support
        using Headers = std::tuple<LineType, LineType, size_t>;
        constexpr std::string_view h1 = "#";
        constexpr std::string_view h2 = "##";
        constexpr std::string_view h3 = "###";

        const auto [header_open, header_close, prefix_size] = [&line, h1, h2, h3]() -> Headers {
            // Literals here means the header type in html and numeric
            // size of prefix in gemtext
            if (LineStartWith(line, h3)) return {"<h3>", "</h3>", 3};
            if (LineStartWith(line, h2)) return {"<h2>", "</h2>", 2};
            if (LineStartWith(line, h1)) return {"<h1>", "</h1>", 1};
        }();

        LineType content = SkipLeadingWs({line.begin() + prefix_size, line.end()});
        if (content.empty()) {
            throw exceptions::HeaderFormatError();
        }

        return header_open + content + header_close;
    }

    GemToHTMLTranslator::LineType GemToHTMLTranslator::LinkTranslator(const LineType &line) const {
        constexpr auto RefFormatter = [](const LineType &ref, const LineType &content) {
            return "<a href=\"" + ref + "\">" + content + "</a>";
        };

        LineType content = SkipLeadingWs({line.begin() + LINK_PREFIX.size(), line.end()});
        if (content.empty()) {
            throw exceptions::LinkFormatError();
        }

        // Compute reference size
        const size_t ref_size = content.find(WS);
        if (ref_size == LineType::npos) {
            return RefFormatter(content, content);
        }

        LineType reference = {content.begin(), content.begin() + ref_size};
        LineType link_text = SkipLeadingWs({content.begin() + ref_size, content.end()});

        return RefFormatter(reference, content);
    }

    GemToHTMLTranslator::LineType GemToHTMLTranslator::ListTranslator(const LineType &line) const {
        static const LineType list_open = "<li>";
        static const LineType list_close = "</li>";
        LineType content = SkipLeadingWs({line.begin() + LIST_PREFIX.size(), line.end()});
        if (content.empty()) {
            throw exceptions::ListFormatError();
        }

        return list_open + content + list_close;
    }

    void GemToHTMLTranslator::WriteHeader(OStreamType &os) const {
        constexpr std::string_view html_header =
            "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "\t<meta charset=\"UTF-8\">\n"
            "\t<title>Title</title>\n"
            "</head>\n"
            "<body>\n";
        std::ostream_iterator<char> it_out{os};
        std::copy(html_header.begin(), html_header.end(), it_out);
    }

    void GemToHTMLTranslator::WriteFooter(OStreamType &os) const {
        constexpr std::string_view html_footer =
            "</body>\n"
            "</html>";
        std::ostream_iterator<char> it_out{os};
        std::copy(html_footer.begin(), html_footer.end(), it_out);
    }
}  // namespace generator
