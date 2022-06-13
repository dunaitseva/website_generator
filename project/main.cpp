#include <fstream>
#include <iostream>
#include <string_view>

#include "FSEntryFinder.hpp"
#include "Generator.hpp"

constexpr size_t EXPECTED_ARGS = 3;
constexpr size_t INPUT_DIR_ARG = 1;
constexpr size_t OUTPUT_DIR_ARG = 2;

void ShowUsage(std::ostream &os) {
    os << "Usage:\n";
    os << "Please enter two arguments. The first argument is the path to the input directory."
          "It should contain the files from which the site structure will be generated (The"
          ".gmi files will be converted to html). The second argument is the output directory"
          "where the site structure with its sources will be placed.\n";
}

int main(int argc, char *argv[]) {
    if (argc != EXPECTED_ARGS) {
        std::cerr << "Error. To few arguments\n";
        ShowUsage(std::cerr);
        return EXIT_FAILURE;
    }

    auto finder = ffinder::CreateFinder<ffinder::RRegularFileFinder>();
    generator::GemtextGenerator generator(finder);

    try {
        generator.Generate(argv[INPUT_DIR_ARG], argv[OUTPUT_DIR_ARG]);
    } catch (const generator::exceptions::DirNotExistError &ex) {
        std::cerr << "Passed wrong directory paths.\n";
    } catch (const generator::exceptions::ErrorFileOpen &ex) {
        std::cerr << "Generation failed. File access error.\n";
    } catch (const generator::exceptions::GemtextFormatError &ex) {
        std::cerr << "Translation error occur. Check your files syntax.\n";
    }

    return EXIT_SUCCESS;
}
