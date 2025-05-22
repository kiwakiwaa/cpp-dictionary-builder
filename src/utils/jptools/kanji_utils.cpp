#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"
#include "utfcpp/utf8.h"

#include <locale>
#include <codecvt>
#include <unordered_map>
#include <unordered_set>

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
        std::u32string result;
        utf8::utf8to32(utf8Str.begin(), utf8Str.end(), std::back_inserter(result));
        return result;
    }

    //convert utf32 to utf8 characters
    std::string utf32ToUtf8(const std::u32string& utf32Str)
    {
        std::string result;
        utf8::utf32to8(utf32Str.begin(), utf32Str.end(), std::back_inserter(result));
        return result;
    }

    bool isKanjiString(const std::string_view text)
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


    std::vector<ResultPair> matchKanaWithKanji(const std::vector<std::string>& entries, int recursionLevel) {
        std::vector<ResultPair> results;
        
        // Early exit if recursion limit is reached
        if (recursionLevel >= 8) {
            for (const auto& entry : entries) {
                results.emplace_back(std::optional(entry), std::nullopt);
            }
            return results;
        }
        
        // Separate entries into kana and kanji
        std::vector<std::string> kana_entries;
        std::vector<std::string> kanji_entries;
        std::vector<std::string> foreign_entries;
        
        for (const auto& entry : entries) {
            std::u32string utf32_entry = utf8ToUtf32(entry);
            
            // Check if entry contains kanji
            bool has_kanji = false;
            for (char32_t c : utf32_entry) {
                if (isKanji(c)) {
                    has_kanji = true;
                    break;
                }
            }
            
            if (has_kanji) {
                kanji_entries.push_back(entry);
            } else if (isHiraganaString(entry) || isKatakanaString(entry) || isHentaiganaString(entry)) {
                kana_entries.push_back(entry);
            } else {
                foreign_entries.push_back(entry);
            }
        }
        
        // Handle foreign entries
        for (const auto& foreign : foreign_entries) {
            results.emplace_back(std::nullopt, std::optional(foreign));
        }
        
        // Check for single kana entry with one or multiple kanji entries
        if (kana_entries.size() == 1 && !kanji_entries.empty()) {
            for (const auto& kanji : kanji_entries) {
                results.emplace_back(std::optional(kanji), std::optional(kana_entries[0]));
            }
            return results;
        }
        
        // Check for kana entries ending with るる and kanji entries ending with るる
        std::vector<std::string> ruru_kana;
        for (const auto& kana : kana_entries) {
            if (kana.size() >= 6) { // Ensure string is long enough for るる (6 bytes in UTF-8)
                if (std::string suffix = kana.substr(kana.size() - 6); suffix == "るる") {
                    ruru_kana.push_back(kana);
                }
            }
        }
        
        if (!ruru_kana.empty()) {
            std::vector<std::string> ruru_kanji;
            for (const auto& kanji : kanji_entries) {
                if (kanji.size() >= 6) {
                    if (std::string suffix = kanji.substr(kanji.size() - 6); suffix == "るる") {
                        ruru_kanji.push_back(kanji);
                    }
                }
            }
            
            if (!ruru_kanji.empty()) {
                // Create pairs between るる kana and るる kanji
                for (const auto& kana : ruru_kana) {
                    for (const auto& kanji : ruru_kanji) {
                        results.emplace_back(std::optional(kanji), std::optional(kana));
                    }
                }
                
                // Remove matched entries to prevent duplicate processing
                std::vector<std::string> remaining_kana;
                for (const auto& kana : kana_entries) {
                    if (std::find(ruru_kana.begin(), ruru_kana.end(), kana) == ruru_kana.end()) {
                        remaining_kana.push_back(kana);
                    }
                }
                
                std::vector<std::string> remaining_kanji;
                for (const auto& kanji : kanji_entries) {
                    if (std::find(ruru_kanji.begin(), ruru_kanji.end(), kanji) == ruru_kanji.end()) {
                        remaining_kanji.push_back(kanji);
                    }
                }
                
                // If we have remaining entries, process them with the regular algorithm
                if (!remaining_kana.empty() || !remaining_kanji.empty()) {
                    std::vector<std::string> remaining_entries;
                    remaining_entries.insert(remaining_entries.end(), remaining_kana.begin(), remaining_kana.end());
                    remaining_entries.insert(remaining_entries.end(), remaining_kanji.begin(), remaining_kanji.end());
                    
                    auto additional_matches = matchKanaWithKanji(remaining_entries, recursionLevel + 1);
                    results.insert(results.end(), additional_matches.begin(), additional_matches.end());
                }
                
                return results;
            }
        }
        
        // Group entries by their non-kanji parts or patterns
        std::unordered_map<std::string, std::vector<std::string>> kanji_groups;
        
        for (const auto& kanji : kanji_entries) {
            std::u32string utf32_kanji = utf8ToUtf32(kanji);
            std::u32string non_kanji_part;
            
            // Extract the non-kanji part
            for (char32_t c : utf32_kanji) {
                if (!isKanji(c)) {
                    non_kanji_part.push_back(c);
                }
            }
            
            // If there's no non-kanji part, use the entire string as a key
            std::string key = non_kanji_part.empty() ? kanji : utf32ToUtf8(non_kanji_part);
            
            kanji_groups[key].push_back(kanji);
        }
        
        // Sets to track matched entries
        std::unordered_set<std::string> matched_kana;
        std::unordered_set<std::string> matched_kanji;
        
        // First pass: match kanji entries with exact non-kanji part matches
        for (const auto& [key, kanji_list] : kanji_groups) {
            auto it = std::find(kana_entries.begin(), kana_entries.end(), key);
            if (it != kana_entries.end()) {
                // Found an exact match
                for (const auto& kanji : kanji_list) {
                    results.emplace_back(std::optional(kanji), std::optional(key));
                    matched_kanji.insert(kanji);
                }
                matched_kana.insert(key);
            }
        }
        
        // Second pass: match entries with similar endings (conjugation forms)
        for (const auto& kana : kana_entries) {
            if (matched_kana.contains(kana)) {
                continue;
            }
            
            std::vector<std::string> best_kanji_matches;
            int best_match_length = 0;
            
            for (const auto& [key, kanji_list] : kanji_groups) {
                // Skip empty non-kanji parts
                if (key.empty()) {
                    continue;
                }
                
                // Find the longest common ending
                int common_length = longestCommonSuffix(kana, key);

                // If we have a substantial match, and it's better than previous matches
                if (common_length >= 1 && common_length > best_match_length) {
                    best_match_length = common_length;
                    best_kanji_matches.clear();
                    
                    for (const auto& kanji : kanji_list) {
                        if (!matched_kanji.contains(kanji)) {
                            best_kanji_matches.push_back(kanji);
                        }
                    }
                } else if (common_length == best_match_length && common_length >= 1) {
                    for (const auto& kanji : kanji_list) {
                        if (!matched_kanji.contains(kanji)) {
                            best_kanji_matches.push_back(kanji);
                        }
                    }
                }
            }
            
            // If we found matches, create entries
            if (!best_kanji_matches.empty()) {
                for (const auto& kanji : best_kanji_matches) {
                    results.emplace_back(std::optional(kanji), std::optional(kana));
                    matched_kanji.insert(kanji);
                }
                matched_kana.insert(kana);
            }
        }
        
        // New pass: match entries with similar prefixes
        for (const auto& kana : kana_entries) {
            if (matched_kana.contains(kana)) {
                continue;
            }
            
            std::vector<std::string> best_kanji_matches;
            int best_match_length = 0;
            
            for (const auto& [key, kanji_list] : kanji_groups) {
                // Skip empty non-kanji parts
                if (key.empty()) {
                    continue;
                }
                
                // Find the longest common prefix
                int common_length = longestCommonPrefix(kana, key);
                
                // If we have a substantial match, and it's better than previous matches
                if (common_length >= 3 && common_length > best_match_length) {
                    best_match_length = common_length;
                    best_kanji_matches.clear();
                    
                    for (const auto& kanji : kanji_list) {
                        if (!matched_kanji.contains(kanji)) {
                            best_kanji_matches.push_back(kanji);
                        }
                    }
                } else if (common_length == best_match_length && common_length >= 3) {
                    for (const auto& kanji : kanji_list) {
                        if (!matched_kanji.contains(kanji)) {
                            best_kanji_matches.push_back(kanji);
                        }
                    }
                }
            }
            
            // If we found matches, create entries
            if (!best_kanji_matches.empty()) {
                for (const auto& kanji : best_kanji_matches) {
                    results.emplace_back(std::optional(kanji), std::optional(kana));
                    matched_kanji.insert(kanji);
                }
                matched_kana.insert(kana);
            }
        }
        
        // Third pass: handle kanji with no non-kanji parts (e.g., "三台" for "さんたい")
        for (const auto& kana : kana_entries) {
            if (matched_kana.contains(kana)) {
                continue;
            }
            
            // Look for kanji entries with no non-kanji part
            for (const auto& kanji : kanji_entries) {
                if (matched_kanji.contains(kanji)) {
                    continue;
                }

                // Check if this is a kanji with no or minimal non-kanji part
                std::u32string utf32_kanji = utf8ToUtf32(kanji);
                if (!utf32_kanji.empty() && isKanji(utf32_kanji.back())) {
                    // Simplified check: if lengths are compatible
                    if (isPlausibleReading(kana, kanji)) {
                        results.emplace_back(std::optional(kanji), std::optional(kana));
                        matched_kanji.insert(kanji);
                        matched_kana.insert(kana);
                        break;  // Move to next kana entry
                    }
                }
            }
        }
        
        // Final pass: look for any pattern matches for remaining entries
        std::vector<std::string> remaining_kanji;
        for (const auto& kanji : kanji_entries) {
            if (!matched_kanji.contains(kanji)) {
                remaining_kanji.push_back(kanji);
            }
        }
        
        std::vector<std::string> remaining_kana;
        for (const auto& kana : kana_entries) {
            if (!matched_kanji.contains(kana)) {
                remaining_kana.push_back(kana);
            }
        }
        
        // If we have remaining entries and haven't exceeded recursion limit
        if (!remaining_kanji.empty() || !remaining_kana.empty()) {
            std::vector<std::string> remaining_entries;
            remaining_entries.insert(remaining_entries.end(), remaining_kanji.begin(), remaining_kanji.end());
            remaining_entries.insert(remaining_entries.end(), remaining_kana.begin(), remaining_kana.end());
            
            // Get additional matches through recursion
            auto additional_matches = matchKanaWithKanji(remaining_entries, recursionLevel + 1);
            results.insert(results.end(), additional_matches.begin(), additional_matches.end());
        } else {
            // Add unmatched entries if we've reached recursion limit
            for (const auto& kana : remaining_kana) {
                results.emplace_back(std::nullopt, std::optional(kana));
            }
            
            for (const auto& kanji : remaining_kanji) {
                results.emplace_back(std::optional(kanji), std::nullopt);
            }
        }
        
        return results;
    }

    std::string extractKanjiStem(const std::string &kanjiEntry)
    {
        const std::u32string u32Str = utf8ToUtf32(kanjiEntry);
        std::u32string result;

        for (const char32_t ch : u32Str)
        {
            if (isKanji(ch))
            {
                result.push_back(ch);
            }
        }
        return utf32ToUtf8(result);
    }

    int longestCommonSuffix(const std::string& str1, const std::string& str2) {
        const std::u32string s1 = utf8ToUtf32(str1);
        const std::u32string s2 = utf8ToUtf32(str2);

        int common_length = 0;
        const int max_possible = std::min(s1.length(), s2.length());

        for (int i = 1; i <= max_possible; ++i) {
            if (s1[s1.length() - i] == s2[s2.length() - i]) {
                common_length = i;
            } else {
                break;
            }
        }

        return common_length;
    }

    int longestCommonPrefix(const std::string& str1, const std::string& str2) {
        const std::u32string s1 = utf8ToUtf32(str1);
        const std::u32string s2 = utf8ToUtf32(str2);

        int common_length = 0;
        const int max_possible = std::min(s1.length(), s2.length());

        for (int i = 0; i < max_possible; ++i) {
            if (s1[i] == s2[i]) {
                common_length++;
            } else {
                break;
            }
        }

        return common_length;
    }

    bool isPlausibleReading(const std::string& kana, const std::string& kanji) {
        const std::u32string utf32_kanji = utf8ToUtf32(kanji);
        const std::u32string utf32_kana = utf8ToUtf32(kana);

        int kanji_chars = 0;
        for (char32_t c : utf32_kanji) {
            if (isKanji(c)) {
                kanji_chars++;
            }
        }

        return utf32_kana.length() >= kanji_chars && utf32_kana.length() <= kanji_chars * 5;
    }
}
