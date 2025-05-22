#include <gtest/gtest.h>

#include "yomitan_dictionary_builder/utils/jptools/kana_convert.h"

TEST(KanaConvertTest, HiraganaToKatakanaTest)
{
    EXPECT_EQ(KanaConvert::hiraganaToKatakana("あいうえお"), "アイウエオ");
    EXPECT_EQ(KanaConvert::hiraganaToKatakana("とよあしはらのみずほのくに"), "トヨアシハラノミズホノクニ");
    EXPECT_EQ(KanaConvert::hiraganaToKatakana("清水の舞台から飛び降りる"), "清水ノ舞台カラ飛ビ降リル");

    EXPECT_FALSE(KanaConvert::hiraganaToKatakana("あいうえお") == "あいうえお");
}

TEST(KanaConvertTest, KatakanaToHiraganaTest)
{
    EXPECT_EQ(KanaConvert::katakanaToHiragana("アイウエオ"), "あいうえお");
    EXPECT_EQ(KanaConvert::katakanaToHiragana("トヨアシハラノミズホノクニ"), "とよあしはらのみずほのくに");
    EXPECT_EQ(KanaConvert::katakanaToHiragana("清水ノ舞台カラ飛ビ降リル"), "清水の舞台から飛び降りる");

    EXPECT_FALSE(KanaConvert::katakanaToHiragana("アイウエオ") == "アイウエオ");
}