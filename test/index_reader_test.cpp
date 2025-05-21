#include <gtest/gtest.h>
#include "yomitan_dictionary_builder/index/index_reader.h"

#include <numeric>

TEST(IndexReaderTest, TestLoadIndex)
{
    constexpr std::string_view kjtIndexPath {"/Users/caoimhe/Documents/日本語/Dictionary Conversion/monokakido-to-yomitan/data/KJT/index/index_d.tsv"};
    IndexReader indexReader{kjtIndexPath};
    EXPECT_TRUE(indexReader.loadIndex());

    const std::vector<std::string> pageNumber = indexReader.getKeysForFile("0000002945");
    EXPECT_TRUE(!std::find(pageNumber.begin(), pageNumber.end(), "星")->empty());
    EXPECT_TRUE(!std::find(pageNumber.begin(), pageNumber.end(), "セイ")->empty());
}