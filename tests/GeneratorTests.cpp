#include <gtest/gtest.h>

#include <string_view>

#include <FSEntryFinder.hpp>
#include <Generator.hpp>

class GemtextGeneratorTests : public ::testing::Test {
 protected:
    ffinder::BasicFSFinder<ffinder::fs::recursive_directory_iterator>::FSFinderShPtr finder;
    ffinder::FSEntityList expected = {
            "../tests/GeneratorTestsData/output/markup.html",
            "../tests/GeneratorTestsData/output/ord_file",
            "../tests/GeneratorTestsData/output/.gitkeep",
            "../tests/GeneratorTestsData/output/subdir/markup2.html",
    };

    generator::GemtextGenerator gemtext_generator;

    static constexpr std::string_view input = "../tests/GeneratorTestsData/input";
    static constexpr std::string_view output = "../tests/GeneratorTestsData/output";

    void SetUp() {
        finder = ffinder::CreateFinder<ffinder::RRegularFileFinder>();
        gemtext_generator.ResetFinder(finder);
    }
};

TEST_F(GemtextGeneratorTests, GenerateValid) {
    gemtext_generator.Generate(input, output);
    ffinder::FSEntityList list = finder->CreateFilesList(output);
    bool verdict = (list == expected);
    ASSERT_TRUE(verdict);
}

TEST_F(GemtextGeneratorTests, InvalidInput) {
    ASSERT_THROW(gemtext_generator.Generate("input", output), generator::exceptions::DirNotExistError);
}

TEST_F(GemtextGeneratorTests, InvalidOutput) {
    ASSERT_THROW(gemtext_generator.Generate(input, "output"), generator::exceptions::DirNotExistError);
}
