#include "kanji_utils.h"

#include <locale>
#include <codecvt>

namespace KanjiUtils
{
    constexpr std::array<std::pair<char32_t, char32_t>, 15> CJK_RANGES = {
        {
            {0x4E00, 0x9FFF},    // CJK Unified Ideographs
            {0x3400, 0x4DBF},    // CJK Unified Ideographs Extension A
            {0x20000, 0x2A6DF},  // CJK Unified Ideographs Extension B
            {0x2A700, 0x2B73F},  // CJK Unified Ideographs Extension C
            {0x2B740, 0x2B81F},  // CJK Unified Ideographs Extension D
            {0x2B820, 0x2CEAF},  // CJK Unified Ideographs Extension E
            {0x2CEB0, 0x2EBEF},  // CJK Unified Ideographs Extension F
            {0x30000, 0x3134F},  // CJK Unified Ideographs Extension G
            {0x31350, 0x323AF},  // CJK Unified Ideographs Extension H
            {0x2EBF0, 0x2EE5F},  // CJK Unified Ideographs Extension I
            {0xF900, 0xFAFF},    // CJK Compatibility Ideographs
            {0x2F800, 0x2FA1F},  // CJK Compatibility Ideographs Supplement
            {0x2F00, 0x2FDF},    // Kangxi Radicals
            {0x2E80, 0x2EFF},    // CJK Radicals Supplement
            {0x2FF0, 0x2FFF},    // Ideographic Description Characters
        }
    };

    constexpr std::array<std::pair<char32_t, char32_t>, 2> HIRAGANA_RANGES = {
        {
            {0x3040, 0x309F},    // Hiragana
            {0x1B132, 0x1B154},  // Historic small hiragana letters
        }
    };

    constexpr std::array<std::pair<char32_t, char32_t>, 5> KATAKANA_RANGES = {
        {
            {0x30A0, 0x30FF},    // Katakana
            {0x31F0, 0x31FF},    // Katakana Phonetic Extensions (from Ainu)
            {0xFF00, 0xFFEF},    // Halfwidth and Fullwidth Forms
            {0x1AFF0, 0x1AFFF},  // Kana Extended-B (Taiwanewse Kana)
            {0x1B155, 0x1B16F},  // Historic small katakana letters
        }
    };

    constexpr std::array<std::pair<char32_t, char32_t>, 2> HENTAIGANA_RANGES = {
        {
            {0x1B100, 0x1B11E},  // Hentaigana
            {0x1B002, 0x1B0FF},  // Hentaigana (from Kana Supplement)
        }
    };

    constexpr std::array<char32_t, 1> SPECIAL_HIRAGANA = {
        0x1B001,    // Hiragana Letter Archaic Ye
    };

    constexpr std::array<char32_t, 4> SPECIAL_KATAKANA = {
        0x1B120,    // Katakana Letter Archaic Yi
        0x1B121,    // Katakana Letter Archaic Ye
        0x1B122,    // Katakana Letter Archaic Wu
        0x1B000,    // Katakana Letter Archaic E
    };

    constexpr std::array<char32_t, 3> SPECIAL_KANJI = {
        0x3005,     // 々 （同の字点）
        0x3007,     // 〇（まる）
        0x303B      // 〻（二の字点）
    };

    bool isKanji(const char32_t ch)
    {
        for (const auto& [start, end] : CJK_RANGES)
        {
            if (ch >= start && ch <= end)
                return true;
        }

        for (const char32_t special : SPECIAL_KANJI)
        {
            if (ch == special)
                return true;
        }

        return false;
    }

    // convert utf8 to utf32 characters
    std::u32string utf8ToUtf32(const std::string_view utf8Str)
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        return converter.from_bytes(utf8Str.data(), utf8Str.data() + utf8Str.size());
    }

    bool isKanjiString(std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (!isKanji(ch))
                return false;
        }
        return true;
    }

    bool containsKanji(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (isKanji(ch))
                return true;
        }
        return false;
    }



    bool isHiragana(const char32_t ch)
    {
        for (const auto& [start, end] : HIRAGANA_RANGES)
        {
            if (ch >= start && ch <= end)
                return true;
        }

        for (const char32_t special : SPECIAL_HIRAGANA)
        {
            if (ch == special)
                return true;
        }

        return false;
    }

    bool isHiraganaString(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (!isHiragana(ch))
                return false;
        }
        return true;
    }

    bool containsHiragana(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (isHiragana(ch))
                return true;
        }
        return false;
    }


    bool isKatakana(const char32_t ch)
    {
        for (const auto& [start, end] : KATAKANA_RANGES)
        {
            if (ch >= start && ch <= end)
                return true;
        }

        for (const char32_t special : SPECIAL_KATAKANA)
        {
            if (ch == special)
                return true;
        }

        return false;
    }

    bool isKatakanaString(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (!isKatakana(ch))
                return false;
        }
        return true;
    }

    bool containsKatakana(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (isKatakana(ch))
                return true;
        }
        return false;
    }



    bool isHentaigana(const char32_t ch)
    {
        for (const auto& [start, end] : HENTAIGANA_RANGES)
        {
            if (ch >= start && ch <= end)
                return true;
        }

        return false;
    }

    bool isHentaiganaString(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (!isHentaigana(ch))
                return false;
        }
        return true;
    }

    bool containsHentaigana(const std::string_view text)
    {
        if (text.empty())
            return false;

        for (const std::u32string utf32Text = utf8ToUtf32(text); const char32_t ch : utf32Text)
        {
            if (isHentaigana(ch))
                return true;
        }
        return false;
    }
}
