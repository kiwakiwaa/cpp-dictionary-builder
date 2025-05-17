#include <gtest/gtest.h>
#include "../src/core/html_element.h"
#include "../src/core/yomitan_dictionary.h"
#include "../src/utils/file_utils.h"

TEST(TEST_JSON, ReadJson)
{
    EXPECT_NO_THROW(FileUtils::readFile("test_index.json"));

    const std::string path {(std::filesystem::current_path().parent_path() / "test/test_index.json").string()};
    auto jsonContent = FileUtils::readFile(path);

    // read_json returns nullptr
    EXPECT_FALSE(glz::read_json(index, *jsonContent)) << "jsonContent = " + *jsonContent;

    const DictionaryIndex index{};
    EXPECT_TRUE(index.revision == "kjt;2025-05-11") << "Revision: " + index.revision;
    EXPECT_TRUE(index.format == 3);
}