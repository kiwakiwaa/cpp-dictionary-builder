#include <gtest/gtest.h>
#include "yomitan_dictionary_builder/index/index_reader.h"

#include <filesystem>
#include <numeric>

TEST(IndexReaderTest, TestLoadIndex)
{
    const auto path = std::filesystem::current_path().parent_path().string() + "/resources/parsers/YDP/index/index_d.tsv";
    IndexReader indexReader{path};
    EXPECT_TRUE(indexReader.loadIndex());

    const std::vector<std::string> pageNumber = indexReader.getKeysForFile("0000001920");
    EXPECT_TRUE(!std::find(pageNumber.begin(), pageNumber.end(), "実験心理学")->empty());
    EXPECT_TRUE(!std::find(pageNumber.begin(), pageNumber.end(), "experimental psychology")->empty());
    EXPECT_TRUE(!std::find(pageNumber.begin(), pageNumber.end(), "ジッケンシンリガク")->empty());
}