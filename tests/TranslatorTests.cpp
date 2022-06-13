#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "Translator.hpp"

using BasicTranslator = generator::BasicTranslator;
using GemToHTMLTranslator = generator::GemToHTMLTranslator;
using DefaultTranslator = generator::DefaultTranslator;

class TranslatorTests : public ::testing::Test {
 protected:
    BasicTranslator::TranslatorShPtr default_translator;
    BasicTranslator::TranslatorShPtr gem_to_html_translator;

    const std::string invalid_input_blockquote = ">";
    const std::string invalid_input_header1 = "#";
    const std::string invalid_input_link = "=>";
    const std::string invalid_input_preformed = "```\nsome";
    const std::string invalid_input_list = "*";

    const std::string valid_input =
        "# Header\n"
        ">Blockquote\n"
        "* list1\n"
        "* list2\n"
        "=> http://some-address.com Line";
    const std::string expected =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<title>Title</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Header</h1>\n"
        "<blockquote><p>Blockquote</p></blockquote>\n"
        "<ul>\n"
        "<li>list1</li>\n"
        "<li>list2</li>\n"
        "</ul>\n"
        "<a href=\"http://some-address.com\">http://some-address.com Line</a>\n"
        "</body>\n"
        "</html>";

    const std::string ends_list = "*list";
    const std::string list_ends_expected =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<title>Title</title>\n"
        "</head>\n"
        "<body>\n"
        "<ul>\n"
        "<li>list</li>\n"
        "</ul>\n"
        "</body>\n"
        "</html>";

    void SetUp() {
        default_translator = generator::CreateTranslator<DefaultTranslator>();
        gem_to_html_translator = generator::CreateTranslator<GemToHTMLTranslator>();
    }
};

TEST_F(TranslatorTests, DefaultTranslate) {
    std::istringstream iss(valid_input);
    std::ostringstream oss;
    default_translator->Translate(iss, oss);
    std::string result = oss.str();
    ASSERT_STREQ(result.c_str(), valid_input.c_str());
}

TEST_F(TranslatorTests, GemToHTMLTranslatorTranslate) {
    std::istringstream iss(valid_input);
    std::ostringstream oss;
    gem_to_html_translator->Translate(iss, oss);
    std::string result = oss.str();
    ASSERT_STREQ(result.c_str(), expected.c_str());
}

TEST_F(TranslatorTests, GemToHTMLTranslatorInvalidBlockquote) {
    std::istringstream iss(invalid_input_blockquote);
    std::ostringstream oss;
    ASSERT_THROW(gem_to_html_translator->Translate(iss, oss), generator::exceptions::BlockquoteFormatError);
}

TEST_F(TranslatorTests, GemToHTMLTranslatorInvalidHeaderEmpty) {
    std::istringstream iss(invalid_input_header1);
    std::ostringstream oss;
    ASSERT_THROW(gem_to_html_translator->Translate(iss, oss), generator::exceptions::HeaderFormatError);
}

TEST_F(TranslatorTests, GemToHTMLTranslatorInvalidLink) {
    std::istringstream iss(invalid_input_link);
    std::ostringstream oss;
    ASSERT_THROW(gem_to_html_translator->Translate(iss, oss), generator::exceptions::LinkFormatError);
}

TEST_F(TranslatorTests, GemToHTMLTranslatorInvalidPreformed) {
    std::istringstream iss(invalid_input_preformed);
    std::ostringstream oss;
    ASSERT_THROW(gem_to_html_translator->Translate(iss, oss), generator::exceptions::PreformedFormatError);
}

TEST_F(TranslatorTests, GemToHTMLTranslatorInvalidList) {
    std::istringstream iss(invalid_input_list);
    std::ostringstream oss;
    ASSERT_THROW(gem_to_html_translator->Translate(iss, oss), generator::exceptions::ListFormatError);
}

TEST_F(TranslatorTests, GemToHTMLTranslatorListEnds) {
    std::istringstream iss(ends_list);
    std::ostringstream oss;
    gem_to_html_translator->Translate(iss, oss);
    std::string result = oss.str();
    ASSERT_STREQ(result.c_str(), list_ends_expected.c_str());
}
