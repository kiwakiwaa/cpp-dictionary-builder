#ifndef KANA_CONVERT_H
#define KANA_CONVERT_H

#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

namespace KanaConvert
{
    /**
     * Converts all hiragana characters to katakana
     * @param text The text to convert
     * @return Text converted to katakana
     */
    std::string hiraganaToKatakana(std::string_view text);

    /**
     * Converts all katakana characters to hiragana
     * @param text The text to convert
     * @return Text converted to hiragana
     */
    std::string katakanaToHiragana(std::string_view text);
}

#endif