#ifndef PROJECT_INCLUDE_FSENTRYFINDER_HPP_
#define PROJECT_INCLUDE_FSENTRYFINDER_HPP_

#include <exception>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <string_view>

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
    using FSEntryList = std::set<FileType>;

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

        BasicFSFinder() = default;

        explicit BasicFSFinder(const PathType &dir_name);

        virtual ~BasicFSFinder() = default;

        /**
         * @return List of paths to files
         *
         * @note CreateFilesList associated with File structure, so it return the list of ones.
         */
        virtual FSEntryList CreateFilesList() const = 0;

     protected:
        PathType m_dir_name;
    };

    template <typename Iter>
    BasicFSFinder<Iter>::BasicFSFinder(const PathType &dir_name) : m_dir_name(dir_name) {
        if (!fs::exists(m_dir_name)) {
            throw exceptions::DirectoryNotFound();
        }

        if (!fs::is_directory(m_dir_name)) {
            throw exceptions::NotDirectory();
        }
    }

    /**
     * In fact, it is the BasicFSFinder for searching regular files and directories.
     */
    template <typename Iter>
    class RegDirBasicFSFinder : public BasicFSFinder<Iter> {
     public:
        using IteratorType = Iter;
        static constexpr std::string_view DefaultPath = ".";

        RegDirBasicFSFinder() : BasicFSFinder<Iter>(DefaultPath) {}

        explicit RegDirBasicFSFinder(const PathType &dir_name) : BasicFSFinder<Iter>(dir_name) {}

        /**
         * Ensures that it returns a list consisting of only regular files and directories.
         * @return List of
         */
        FSEntryList CreateFilesList() const override;
    };

    template <typename Iter>
    FSEntryList RegDirBasicFSFinder<Iter>::CreateFilesList() const {
        FSEntryList regular_files_list;
        // Iterate over directory and find all regular files and directories.
        for (const auto &path_entry : IteratorType{BasicFSFinder<Iter>::m_dir_name}) {
            if (IsRegular(path_entry) || IsDirectory(path_entry)) {
                regular_files_list.emplace(fs::absolute(path_entry));
            }
        }
        return regular_files_list;
    }

    using RegualarFileFinder = RegDirBasicFSFinder<fs::directory_iterator>;
    using RRegualarFileFinder = RegDirBasicFSFinder<fs::recursive_directory_iterator>;
}  // namespace ffinder

#endif  // PROJECT_INCLUDE_FSENTRYFINDER_HPP_
