#include <gtest/gtest.h>
#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"
#include <iostream>

TEST(TEST_JSON, ReadJson)
{
    EXPECT_NO_THROW(FileUtils::readFile("test_index.json"));

    const std::string path {(std::filesystem::current_path().parent_path() / "test/test_index.json").string()};
    auto jsonContent = FileUtils::readFile(path);
    DictionaryIndex index{};

    // read_json returns nullptr
    EXPECT_FALSE(glz::read_json(index, *jsonContent));
    EXPECT_TRUE(index.revision == "kjt;2025-05-11") << "\nActual revision: " + index.revision;
    EXPECT_TRUE(index.format == 3) << "\nActual format: " + index.format;
}