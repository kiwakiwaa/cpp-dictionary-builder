#ifndef KANJI_UTILS_H
#define KANJI_UTILS_H

#include <regex>
#include <string>
#include <string_view>
#include <optional>

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

    /**
     * Check if a string is Kanji-only
     *
     * The function determines if a string only includes kanji
     * @param text The string to check
     * @return true if all the characters in the string is a kanji
     */
    bool isKanjiString(std::string_view text);

    /**
     * Check if a string contains any kanji
     *
     * The function determines if a string contains any kanji
     * @param text The string to check
     * @return true if any of the characters is a kanji
     */
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

    /**
     * Helper method to convert a string from UTF-8 to UTF-32
     * @param utf8Str The UTF-8 encoded string to convert
     * @return UTF-32 encoded string
     */
    std::u32string utf8ToUtf32(std::string_view utf8Str);

    /**
     * Helper method to convert a string from UTF-32 to UTF-8
     * @param utf32Str The UTF-32 encoded string to convert
     * @return UTF-8 encoded string
     */
    std::string utf32ToUtf8(const std::u32string& utf32Str);

    std::string extractKanjiStem(const std::string& kanjiEntry);

    int longestCommonPrefix(const std::string& str1, const std::string& str2);

    int longestCommonSuffix(const std::string& str1, const std::string& str2);

    bool isPlausibleReading(const std::string& kana, const std::string& kanji);

    using ResultPair = std::pair<std::optional<std::string>, std::optional<std::string>>;

    /**
     * Processes a vector of entry keys and matches their kanji and kana keys
     * @param entries The vector of entry keys to match
     * @param recursionLevel Current recursion depth (maximum 8)
     * @return A vector of matched key pairs
     */
    std::vector<ResultPair> matchKanaWithKanji(const std::vector<std::string>& entries, int recursionLevel = 0);


}


#endif
