#ifndef PROJECT_INCLUDE_FSENTRYFINDER_HPP_
#define PROJECT_INCLUDE_FSENTRYFINDER_HPP_

#include <exception>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <utility>

namespace ffinder {
    namespace fs = std::filesystem;
    using PathType = fs::path;

    bool IsRegular(const PathType &file);
    bool IsDirectory(const PathType &file);

    namespace exceptions {
        class FinderException : std::exception {
         public:
            const char *what() const noexcept override { return "Finder exception occur"; }
        };

        class DirectoryNotFound : FinderException {
         public:
            const char *what() const noexcept override { return "Specified directory not found"; }
        };

        class NotDirectory : FinderException {
         public:
            const char *what() const noexcept override { return "Specified filesystem object is not a directory"; }
        };
    }  // namespace exceptions

    using FileType = PathType;
    using FSEntityList = std::set<FileType>;

    /**
     * Basic class, that provides an interface for creating classes that return a list of
     * files contained in a directory, with additional custom logic.
     *
     * @tparam Iter type of directory iterator.
     *
     * @note In fact, Iter should be std::filesystem::directory_iterator or
     * std::filesystem::recursive::directory_iterator.
     */
    template <typename Iter>
    class BasicFSFinder {
     public:
        using IteratorType = Iter;
        using FSFinderShPtr = std::shared_ptr<BasicFSFinder>;
        using FSFinderWkPtr = std::weak_ptr<BasicFSFinder>;

        static constexpr std::string_view DefaultPath = ".";

        BasicFSFinder() = default;

        virtual ~BasicFSFinder() = default;

        /**
         * Iterate over directory and creates list of files.
         *
         * @paragraph dir_name Target directory.
         *
         * @return List of paths to files
         * @note CreateFilesList associated with File structure, so it return the list of ones.
         */
        virtual FSEntityList CreateFilesList(const PathType &dir_name) const = 0;
        static void CheckExistence(const PathType &dir_name);
    };

    template <typename Iter>
    void BasicFSFinder<Iter>::CheckExistence(const PathType &dir_name) {
        if (!fs::exists(dir_name)) {
            throw exceptions::DirectoryNotFound();
        }

        if (!fs::is_directory(dir_name)) {
            throw exceptions::NotDirectory();
        }
    }

    /**
     * In fact, it is the BasicFSFinder for searching regular files and directories.
     */
    template <typename Iter>
    class RegularBasicFSFinder : public BasicFSFinder<Iter> {
     public:
        using IteratorType = Iter;
        static constexpr std::string_view DefaultPath = ".";

        RegularBasicFSFinder() = default;

        /**
         * Ensures that it returns a list consisting of only regular files and directories.
         * @return List of
         */
        FSEntityList CreateFilesList(const PathType &dir_name) const override;
    };

    template <typename Iter>
    FSEntityList RegularBasicFSFinder<Iter>::CreateFilesList(const PathType &dir_name) const {
        BasicFSFinder<Iter>::CheckExistence(dir_name);
        FSEntityList regular_files_list;
        // Iterate over directory and find all regular files and directories.
        for (const auto &path_entry : IteratorType{dir_name}) {
            if (IsRegular(path_entry)) {
                //                regular_files_list.emplace(fs::absolute(path_entry));
                regular_files_list.emplace(path_entry);
            }
        }
        return regular_files_list;
    }

    using RegualrFileFinder = RegularBasicFSFinder<fs::directory_iterator>;
    using RRegularFileFinder = RegularBasicFSFinder<fs::recursive_directory_iterator>;

    /**
     * Creates a pointer to finder specified object.
     * @tparam FinderType Finder type.
     * @tparam Iter Directory iterator type.
     * @tparam Args Finder constructor arguments types.
     * @param args Finder constructor arguments.
     * @return Pointer to finder.
     */
    template <typename FinderType, typename Iter = typename FinderType::IteratorType, typename... Args>
    typename BasicFSFinder<Iter>::FSFinderShPtr CreateFinder(Args &&...args) {
        return std::make_unique<FinderType>(std::forward<Args>(args)...);
    }
}  // namespace ffinder

#endif  // PROJECT_INCLUDE_FSENTRYFINDER_HPP_
