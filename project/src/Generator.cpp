#include "Generator.hpp"

#include <filesystem>
#include <fstream>

#include "FSEntryFinder.hpp"

namespace generator {
    namespace fs = std::filesystem;

    void GemtextGenerator::CheckStreams(const std::ifstream &ifs, const std::ofstream &ofs) {
        if (!ifs.is_open() || !ofs.is_open()) {
            throw exceptions::ErrorFileOpen();
        }
    }

    ffinder::FSEntityList BasicWebsiteGenerator::LoadInputDirectory(const ffinder::PathType &input_dir) const {
        return m_finder->CreateFilesList(input_dir);
    }

    void GemtextGenerator::Generate(const ffinder::PathType &input_dir, const ffinder::PathType &output_dir) {
        constexpr fs::copy_options copy_opts_files = fs::copy_options::overwrite_existing;
        constexpr fs::copy_options copy_opts_dirs = fs::copy_options::recursive | fs::copy_options::directories_only;
        if (!IsExists(input_dir) || !IsExists(output_dir)) {
            throw exceptions::DirNotExistError();
        }

        fs::copy(input_dir, output_dir, copy_opts_dirs);

        auto entities = LoadInputDirectory(input_dir);
        for (const auto file : entities) {
            ffinder::PathType rel_to_input_path = fs::relative(file, input_dir);
            if (file.extension() != GEM_EXT) {
                fs::copy(file, output_dir / rel_to_input_path, copy_opts_files);
            } else {
                // Create file with new extension
                ffinder::PathType file_new_extension = rel_to_input_path;
                file_new_extension.replace_extension(HTML_EXT);
                std::ofstream ofs(output_dir / file_new_extension);
                std::ifstream ifs(file);
                CheckStreams(ifs, ofs);
                auto translator = GetTranslator(file);
                translator->Translate(ifs, ofs);
            }
        }
    }

    BasicTranslator::TranslatorShPtr GemtextGenerator::GetTranslator(const ffinder::PathType &file) {
        if (file.extension() == GEM_EXT) {
            return CreateTranslator<GemToHTMLTranslator>();
        }

        return CreateTranslator<DefaultTranslator>();
    }
}  // namespace generator
