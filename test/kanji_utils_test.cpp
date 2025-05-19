#include <gtest/gtest.h>
#include "../src/utils/jptools/kanji_utils.h"

TEST(KanjiUtilsTest, SingleCharacterTests)
{
    // ---- 漢字 ---- //
    EXPECT_TRUE(KanjiUtils::isKanji(U'水'));
    EXPECT_TRUE(KanjiUtils::isKanji(U'鼬'));
    EXPECT_TRUE(KanjiUtils::isKanji(0x30EDE)); // 𰻞（ビャン）
    EXPECT_TRUE(KanjiUtils::isKanji(U'雹'));

    // Special
    EXPECT_TRUE(KanjiUtils::isKanji(U'々'));
    EXPECT_TRUE(KanjiUtils::isKanji(U'〻'));

    // Non kanji
    EXPECT_FALSE(KanjiUtils::isKanji(U'あ'));
    EXPECT_FALSE(KanjiUtils::isKanji(U'ア'));
    EXPECT_FALSE(KanjiUtils::isKanji('A'));
    EXPECT_FALSE(KanjiUtils::isKanji('1'));
    EXPECT_FALSE(KanjiUtils::isKanji('!'));


    // ---- 平仮名 ---- //
    EXPECT_TRUE(KanjiUtils::isHiragana(U'を'));
    EXPECT_TRUE(KanjiUtils::isHiragana(U'𛀁'));

    EXPECT_FALSE(KanjiUtils::isHiragana(U'霰'));
    EXPECT_FALSE(KanjiUtils::isHiragana('A'));
    EXPECT_FALSE(KanjiUtils::isHiragana('1'));
    EXPECT_FALSE(KanjiUtils::isHiragana('!'));


    // ---- 片仮名 ---- //
    EXPECT_TRUE(KanjiUtils::isKatakana(U'ヲ'));
    EXPECT_TRUE(KanjiUtils::isKatakana(U'𛀀'));

    EXPECT_FALSE(KanjiUtils::isKatakana(U'霰'));
    EXPECT_FALSE(KanjiUtils::isKatakana('A'));
    EXPECT_FALSE(KanjiUtils::isKatakana('1'));
    EXPECT_FALSE(KanjiUtils::isKatakana('!'));


    // ---- 変体仮名 ---- //
    EXPECT_TRUE(KanjiUtils::isHentaigana(U'𛄀'));
    EXPECT_TRUE(KanjiUtils::isHentaigana(U'𛄐'));
    EXPECT_TRUE(KanjiUtils::isHentaigana(U'𛄞'));
    EXPECT_TRUE(KanjiUtils::isHentaigana(U'𛀂'));
    EXPECT_TRUE(KanjiUtils::isHentaigana(U'𛃾'));

    EXPECT_FALSE(KanjiUtils::isHentaigana(U'霰'));
    EXPECT_FALSE(KanjiUtils::isHentaigana(U'あ'));
    EXPECT_FALSE(KanjiUtils::isHentaigana(U'ア'));
    EXPECT_FALSE(KanjiUtils::isHentaigana('A'));
    EXPECT_FALSE(KanjiUtils::isHentaigana('1'));
    EXPECT_FALSE(KanjiUtils::isHentaigana('!'));
}

TEST(KanjiUtilsTest, StringTests)
{
    // ---- 漢字 ---- //
    EXPECT_TRUE(KanjiUtils::isKanjiString("漢字"));
    EXPECT_TRUE(KanjiUtils::isKanjiString("甲乙丙丁"));
    EXPECT_TRUE(KanjiUtils::isKanjiString("豊葦原瑞穂国"));

    EXPECT_FALSE(KanjiUtils::isKanjiString("清水の舞台から飛び降りる"));
    EXPECT_FALSE(KanjiUtils::isKanjiString("hellouuu"));
    EXPECT_FALSE(KanjiUtils::isKanjiString("123"));
    EXPECT_FALSE(KanjiUtils::isKanjiString(""));


    // ---- 平仮名 ---- //
    EXPECT_TRUE(KanjiUtils::isHiraganaString("あべのせいめい"));
    EXPECT_TRUE(KanjiUtils::isHiraganaString("𛀁あい"));
    EXPECT_TRUE(KanjiUtils::isHiraganaString("とよあしはらのみずほのくに"));

    EXPECT_FALSE(KanjiUtils::isHiraganaString("清水ノ舞台カラ飛ビ降リル"));
    EXPECT_FALSE(KanjiUtils::isHiraganaString("清水の舞台から飛び降りる"));
    EXPECT_FALSE(KanjiUtils::isHiraganaString("hellouuu"));
    EXPECT_FALSE(KanjiUtils::isHiraganaString("123"));
    EXPECT_FALSE(KanjiUtils::isHiraganaString(""));


    // ---- 片仮名 ---- //
    EXPECT_TRUE(KanjiUtils::isKatakanaString("アベノセイメイ"));
    EXPECT_TRUE(KanjiUtils::isKatakanaString("𛀀アイ"));
    EXPECT_TRUE(KanjiUtils::isKatakanaString("ﾄﾖｱｼﾊﾗﾉﾐｽﾞﾎﾉｸﾆ"));
    EXPECT_TRUE(KanjiUtils::isKatakanaString("トヨアシハラノミズホノクニ"));

    EXPECT_FALSE(KanjiUtils::isKatakanaString("清水ノ舞台カラ飛ビ降リル"));
    EXPECT_FALSE(KanjiUtils::isKatakanaString("清水の舞台から飛び降りる"));
    EXPECT_FALSE(KanjiUtils::isKatakanaString("hellouuu"));
    EXPECT_FALSE(KanjiUtils::isKatakanaString("123"));
    EXPECT_FALSE(KanjiUtils::isKatakanaString(""));
}

TEST(KanjiUtilsTest, ContainsKanjiTest)
{
    // --- 漢字 ---- //
    EXPECT_TRUE(KanjiUtils::containsKanji("豊葦原瑞穂国"));
    EXPECT_TRUE(KanjiUtils::containsKanji("清水の舞台から飛び降りる"));

    EXPECT_FALSE(KanjiUtils::containsKanji("あいうえお"));
    EXPECT_FALSE(KanjiUtils::containsKanji("helllloouuuuu"));
    EXPECT_FALSE(KanjiUtils::containsKanji("123"));
    EXPECT_FALSE(KanjiUtils::containsKanji(""));


    // --- 平仮名 ---- //
    EXPECT_TRUE(KanjiUtils::containsHiragana("あべのせいめい"));
    EXPECT_TRUE(KanjiUtils::containsHiragana("𛀁あい"));
    EXPECT_TRUE(KanjiUtils::containsHiragana("とよあしはらのみずほのくに"));
    EXPECT_TRUE(KanjiUtils::containsHiragana("清水の舞台から飛び降りる"));

    EXPECT_FALSE(KanjiUtils::containsHiragana("清水ノ舞台カラ飛ビ降リル"));
    EXPECT_FALSE(KanjiUtils::containsHiragana("helllloouuuuu"));
    EXPECT_FALSE(KanjiUtils::containsHiragana("123"));
    EXPECT_FALSE(KanjiUtils::containsHiragana(""));


    // --- 片仮名 ---- //
    EXPECT_TRUE(KanjiUtils::containsKatakana("アベノセイメイ"));
    EXPECT_TRUE(KanjiUtils::containsKatakana("𛀀アイ"));
    EXPECT_TRUE(KanjiUtils::containsKatakana("ﾄﾖｱｼﾊﾗﾉﾐｽﾞﾎﾉｸﾆに行きたい"));
    EXPECT_TRUE(KanjiUtils::containsKatakana("トヨアシハラノミズホノクニにいきたい"));

    EXPECT_FALSE(KanjiUtils::containsKatakana("清水の舞台から飛び降りる"));
    EXPECT_FALSE(KanjiUtils::containsKatakana("helllloouuuuu"));
    EXPECT_FALSE(KanjiUtils::containsKatakana("123"));
    EXPECT_FALSE(KanjiUtils::containsKatakana(""));
}