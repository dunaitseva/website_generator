#include <fstream>
#include <iostream>
#include <string_view>

#include "Translator.hpp"

constexpr std::string_view file = "../tmp/some.gem";
constexpr std::string_view ofile = "../tmp/some.html";

int main() {
    namespace gen = generator;

    std::ifstream ifs;
    ifs.open(file.data());
    if (!ifs.is_open()) {
        std::cerr << "ERROR" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream ofs;
    ofs.open(ofile.data(), std::ios::trunc);
    if (!ofs.is_open()) {
        std::cerr << "ERROR" << std::endl;
        return EXIT_FAILURE;
    }

    gen::BasicTranslator::TranslatorShPtr translator = gen::CreateTranslator<gen::GemToHTMLTranslator>();
    translator->Translate(ifs, ofs);
    return EXIT_SUCCESS;
}
