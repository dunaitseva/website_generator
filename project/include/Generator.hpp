#ifndef PROJECT_INCLUDE_GENERATOR_HPP_
#define PROJECT_INCLUDE_GENERATOR_HPP_

#include <exception>
#include <filesystem>
#include <fstream>
#include <string_view>

#include "FSEntryFinder.hpp"
#include "Translator.hpp"

namespace generator {
    namespace exceptions {
        class GeneratorError : std::exception {
         public:
            const char *what() const noexcept override { return "GeneratorError occur."; }
        };

        class DirNotExistError : GeneratorError {
         public:
            const char *what() const noexcept override { return "Passed directory does not exist."; }
        };

        class ErrorFileOpen : GeneratorError {
         public:
            const char *what() const noexcept override { return "ErrorFileOpen occur."; }

         private:
            std::string_view m_file_name;
            std::string_view message;
        };
    }  // namespace exceptions

    class BasicWebsiteGenerator {
     public:
        using DirectoryIter = std::filesystem::recursive_directory_iterator;
        using FSFinderPtrType = ffinder::BasicFSFinder<DirectoryIter>::FSFinderShPtr;

        BasicWebsiteGenerator() = default;

        explicit BasicWebsiteGenerator(const FSFinderPtrType &finder) : m_finder(finder) {}

        /**
         * Iterates over all entities in the input directory and generates entities in the output directory.
         * The result of generating a file depends on which translator the file was translated with.
         * The choice of translator depends on the type of generator and is set in the virtual function
         * CreateTranslatorByExtension.
         */
        virtual void Generate(const ffinder::PathType &input_dir, const ffinder::PathType &output_dir) = 0;

        void ResetFinder(const FSFinderPtrType &finder) { m_finder = finder; }

        virtual ~BasicWebsiteGenerator() = default;

     protected:
        /**
         * Allows you to select a translator depending on the file.
         * @param file Path to file.
         * @return Translator
         */
        virtual BasicTranslator::TranslatorShPtr GetTranslator(const ffinder::PathType &file) = 0;

        /**
         * Load input directory entries. Accessor to generator's finder.
         * @param input_dir Directory, which entities you want to collect
         */
        ffinder::FSEntityList LoadInputDirectory(const ffinder::PathType &input_dir) const;

        static bool IsExists(const ffinder::PathType &path) { return std::filesystem::exists(path); }

     private:
        FSFinderPtrType m_finder;
    };

    class GemtextGenerator : public BasicWebsiteGenerator {
     public:
        static constexpr std::string_view GEM_EXT = ".gmi";
        static constexpr std::string_view HTML_EXT = ".html";

        GemtextGenerator() = default;

        explicit GemtextGenerator(const FSFinderPtrType &finder) : BasicWebsiteGenerator(finder) {}

        void Generate(const ffinder::PathType &input_dir, const ffinder::PathType &output_dir) override;

     protected:
        BasicTranslator::TranslatorShPtr GetTranslator(const ffinder::PathType &file) override;

     private:
        static void CheckStreams(const std::ifstream &ifs, const std::ofstream &ofs);
    };
}  // namespace generator

#endif  // PROJECT_INCLUDE_GENERATOR_HPP_
