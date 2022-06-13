#include <FSEntryFinder.hpp>

namespace ffinder {
    namespace fs = std::filesystem;

    bool IsRegular(const PathType &file) { return fs::is_regular_file(file) && !fs::is_symlink(file); }
    bool IsDirectory(const PathType &file) { return std::filesystem::is_directory(file); }
}  // namespace ffinder
