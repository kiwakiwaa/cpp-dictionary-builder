#ifndef KANJI_UTILS_H
#define KANJI_UTILS_H

#include <regex>
#include <string>
#include <string_view>
#include <array>

namespace KanjiUtils
{
    /**
     * Checks if a character is a Kanji (CJK)
     *
     * The function determines if a Unicode character falls within any of the
     * CJK Ideograph blocks.
     * @param ch The character to check
     * @return true if the character is a kanji
     */
    bool isKanji(char32_t ch);
    bool isKanjiString(std::string_view text);
    bool containsKanji(std::string_view text);

    bool isKatakana(char32_t ch);
    bool isKatakanaString(std::string_view text);
    bool containsKatakana(std::string_view text);

    bool isHiragana(char32_t ch);
    bool isHiraganaString(std::string_view text);
    bool containsHiragana(std::string_view text);

    bool isHentaigana(char32_t ch);
    bool isHentaiganaString(std::string_view text);
    bool containsHentaigana(std::string_view text);
}


#endif
