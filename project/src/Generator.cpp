#include "Generator.hpp"

#include <filesystem>

#include "FSEntryFinder.hpp"

namespace generator {
    BasicGenerator::BasicGenerator(const ffinder::PathType &input_dir, const ffinder::PathType &output_dir)
        : m_entries_list() {
        if (!std::filesystem::exists(input_dir) || !std::filesystem::exists(output_dir)) {
            throw exceptions::DirNotExistError();
        }
        m_input_dir = input_dir;
        m_output_dir = output_dir;
    }

    template <typename Iter>
    void BasicGenerator::LoadInputDirectory(const ffinder::BasicFSFinder<Iter> &finder) {
        m_entries_list = finder.CreateFilesList();
    }

    void BasicGenerator::Generate() {}
}  // namespace generator
