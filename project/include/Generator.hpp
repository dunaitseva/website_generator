#ifndef PROJECT_INCLUDE_GENERATOR_HPP_
#define PROJECT_INCLUDE_GENERATOR_HPP_

#include <exception>

#include "FSEntryFinder.hpp"
#include "Translator.hpp"

namespace generator {
    namespace exceptions {
        class GeneratorError : std::exception {
            const char *what() const noexcept override { return "GeneratorError occur."; }
        };

        class DirNotExistError : GeneratorError {
            const char *what() const noexcept override { return "Passed directory does not exist."; }
        };
    }  // namespace exceptions

    class BasicGenerator {
     public:
        BasicGenerator(const ffinder::PathType &input_dir, const ffinder::PathType &output_dir);

        /**
         * Iterates over all entities in the directory and generates entities in the output directory.
         * The result of generating a file depends on which translator the file was translated with.
         * The choice of translator depends on the type of generator and is set in the virtual function
         * CreateTranslatorByExtension.
         */
        void Generate();

        /**
         * Load input directory entries.
         * @param finder Finder for files in a directory
         * @param Iter Directory iterator type
         */
        template <class Iter>
        void LoadInputDirectory(const ffinder::BasicFSFinder<Iter> &finder);

     protected:
        /**
         * Allows you to select a translator depending on the file.
         * @param file Path to file.
         * @return Translator
         */
        virtual BasicTranslator::TranslatorShPtr CreateTranslator(const ffinder::PathType &file) = 0;

     private:
        ffinder::PathType m_input_dir;
        ffinder::PathType m_output_dir;
        ffinder::FSEntryList m_entries_list;
    };
}  // namespace generator

#endif  // PROJECT_INCLUDE_GENERATOR_HPP_
