#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>

#include "FSEntryFinder.hpp"

class RegDirFinderTests : public ::testing::Test {
 protected:
    static constexpr std::string_view file1 = "../tests/FSEntryFinderTestData/file1";
    static constexpr std::string_view file2 = "../tests/FSEntryFinderTestData/dir1/file2";
    static constexpr std::string_view file3 = "../tests/FSEntryFinderTestData/dir2/file3";
    static constexpr std::string_view file4 = "../tests/FSEntryFinderTestData/dir2/dirdir1/file4";
    static constexpr std::string_view dir1 = "../tests/FSEntryFinderTestData/dir1";
    static constexpr std::string_view dir2 = "../tests/FSEntryFinderTestData/dir2";
    static constexpr std::string_view dirdir1 = "../tests/FSEntryFinderTestData/dir2/dirdir1";

    static constexpr std::string_view valid_path = "../tests/FSEntryFinderTestData";

    ffinder::FSEntryList expected_list;

    void SetUp() {
        expected_list.emplace(std::filesystem::absolute(file1));
        expected_list.emplace(std::filesystem::absolute(file2));
        expected_list.emplace(std::filesystem::absolute(file3));
        expected_list.emplace(std::filesystem::absolute(file4));
        expected_list.emplace(std::filesystem::absolute(dir1));
        expected_list.emplace(std::filesystem::absolute(dir2));
        expected_list.emplace(std::filesystem::absolute(dirdir1));
    }
};

TEST_F(RegDirFinderTests, ValidDirectory) { ASSERT_NO_THROW(ffinder::RRegualarFileFinder finder(valid_path)); }

TEST_F(RegDirFinderTests, NotExistDir) {
    ASSERT_THROW(ffinder::RRegualarFileFinder finder("valid_path"), ffinder::exceptions::DirectoryNotFound);
}

TEST_F(RegDirFinderTests, NotDirectory) {
    ASSERT_THROW(ffinder::RRegualarFileFinder finder(file1), ffinder::exceptions::NotDirectory);
}

TEST_F(RegDirFinderTests, DirectoryRecursiveTraverse) {
    ffinder::RRegualarFileFinder finder(valid_path);
    ffinder::FSEntryList result_list = finder.CreateFilesList();
    bool verdict = (result_list == expected_list);
    ASSERT_TRUE(verdict);
}
