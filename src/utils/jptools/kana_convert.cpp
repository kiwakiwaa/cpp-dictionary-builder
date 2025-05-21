#include "yomitan_dictionary_builder/utils/jptools/kana_convert.h"

#include <unordered_map>

namespace KanaConvert
{
    const std::unordered_map<char32_t, char32_t> HIRAGANA_TO_KATAKANA = {
        {0x3042, 0x30A2}, // あ -> ア
        {0x3044, 0x30A4}, // い -> イ
        {0x3046, 0x30A6}, // う -> ウ
        {0x3048, 0x30A8}, // え -> エ
        {0x304A, 0x30AA}, // お -> オ
        {0x304B, 0x30AB}, // か -> カ
        {0x304D, 0x30AD}, // き -> キ
        {0x304F, 0x30AF}, // く -> ク
        {0x3051, 0x30B1}, // け -> ケ
        {0x3053, 0x30B3}, // こ -> コ
        {0x3055, 0x30B5}, // さ -> サ
        {0x3057, 0x30B7}, // し -> シ
        {0x3059, 0x30B9}, // す -> ス
        {0x305B, 0x30BB}, // せ -> セ
        {0x305D, 0x30BD}, // そ -> ソ
        {0x305F, 0x30BF}, // た -> タ
        {0x3061, 0x30C1}, // ち -> チ
        {0x3064, 0x30C4}, // つ -> ツ
        {0x3066, 0x30C6}, // て -> テ
        {0x3068, 0x30C8}, // と -> ト
        {0x306A, 0x30CA}, // な -> ナ
        {0x306B, 0x30CB}, // に -> ニ
        {0x306C, 0x30CC}, // ぬ -> ヌ
        {0x306D, 0x30CD}, // ね -> ネ
        {0x306E, 0x30CE}, // の -> ノ
        {0x306F, 0x30CF}, // は -> ハ
        {0x3072, 0x30D2}, // ひ -> ヒ
        {0x3075, 0x30D5}, // ふ -> フ
        {0x3078, 0x30D8}, // へ -> ヘ
        {0x307B, 0x30DB}, // ほ -> ホ
        {0x307E, 0x30DE}, // ま -> マ
        {0x307F, 0x30DF}, // み -> ミ
        {0x3080, 0x30E0}, // む -> ム
        {0x3081, 0x30E1}, // め -> メ
        {0x3082, 0x30E2}, // も -> モ
        {0x3084, 0x30E4}, // や -> ヤ
        {0x3086, 0x30E6}, // ゆ -> ユ
        {0x3088, 0x30E8}, // よ -> ヨ
        {0x3089, 0x30E9}, // ら -> ラ
        {0x308A, 0x30EA}, // り -> リ
        {0x308B, 0x30EB}, // る -> ル
        {0x308C, 0x30EC}, // れ -> レ
        {0x308D, 0x30ED}, // ろ -> ロ
        {0x308F, 0x30EF}, // わ -> ワ
        {0x3092, 0x30F2}, // を -> ヲ
        {0x3093, 0x30F3}, // ん -> ン
        {0x304C, 0x30AC}, // が -> ガ
        {0x304E, 0x30AE}, // ぎ -> ギ
        {0x3050, 0x30B0}, // ぐ -> グ
        {0x3052, 0x30B2}, // げ -> ゲ
        {0x3054, 0x30B4}, // ご -> ゴ
        {0x3056, 0x30B6}, // ざ -> ザ
        {0x3058, 0x30B8}, // じ -> ジ
        {0x305A, 0x30BA}, // ず -> ズ
        {0x305C, 0x30BC}, // ぜ -> ゼ
        {0x305E, 0x30BE}, // ぞ -> ゾ
        {0x3060, 0x30C0}, // だ -> ダ
        {0x3062, 0x30C2}, // ぢ -> ヂ
        {0x3065, 0x30C5}, // づ -> ヅ
        {0x3067, 0x30C7}, // で -> デ
        {0x3069, 0x30C9}, // ど -> ド
        {0x3070, 0x30D0}, // ば -> バ
        {0x3073, 0x30D3}, // び -> ビ
        {0x3076, 0x30D6}, // ぶ -> ブ
        {0x3079, 0x30D9}, // べ -> ベ
        {0x307C, 0x30DC}, // ぼ -> ボ
        {0x3071, 0x30D1}, // ぱ -> パ
        {0x3074, 0x30D4}, // ぴ -> ピ
        {0x3077, 0x30D7}, // ぷ -> プ
        {0x307A, 0x30DA}, // ぺ -> ペ
        {0x307D, 0x30DD}, // ぽ -> ポ
        {0x3083, 0x30E3}, // ゃ -> ャ
        {0x3085, 0x30E5}, // ゅ -> ュ
        {0x3087, 0x30E7}, // ょ -> ョ
        {0x3063, 0x30C3}, // っ -> ッ
        {0x308E, 0x30EE}, // ゎ -> ヮ
        {0x3090, 0x30F0}, // ゐ -> ヰ
        {0x3091, 0x30F1}  // ゑ -> ヱ
    };

    const std::unordered_map<char32_t, char32_t> KATAKANA_TO_HIRAGANA = {
        {0x30A2, 0x3042}, // ア -> あ
        {0x30A4, 0x3044}, // イ -> い
        {0x30A6, 0x3046}, // ウ -> う
        {0x30A8, 0x3048}, // エ -> え
        {0x30AA, 0x304A}, // オ -> お
        {0x30AB, 0x304B}, // カ -> か
        {0x30AD, 0x304D}, // キ -> き
        {0x30AF, 0x304F}, // ク -> く
        {0x30B1, 0x3051}, // ケ -> け
        {0x30B3, 0x3053}, // コ -> こ
        {0x30B5, 0x3055}, // サ -> さ
        {0x30B7, 0x3057}, // シ -> し
        {0x30B9, 0x3059}, // ス -> す
        {0x30BB, 0x305B}, // セ -> せ
        {0x30BD, 0x305D}, // ソ -> そ
        {0x30BF, 0x305F}, // タ -> た
        {0x30C1, 0x3061}, // チ -> ち
        {0x30C4, 0x3064}, // ツ -> つ
        {0x30C6, 0x3066}, // テ -> て
        {0x30C8, 0x3068}, // ト -> と
        {0x30CA, 0x306A}, // ナ -> な
        {0x30CB, 0x306B}, // ニ -> に
        {0x30CC, 0x306C}, // ヌ -> ぬ
        {0x30CD, 0x306D}, // ネ -> ね
        {0x30CE, 0x306E}, // ノ -> の
        {0x30CF, 0x306F}, // ハ -> は
        {0x30D2, 0x3072}, // ヒ -> ひ
        {0x30D5, 0x3075}, // フ -> ふ
        {0x30D8, 0x3078}, // ヘ -> へ
        {0x30DB, 0x307B}, // ホ -> ほ
        {0x30DE, 0x307E}, // マ -> ま
        {0x30DF, 0x307F}, // ミ -> み
        {0x30E0, 0x3080}, // ム -> む
        {0x30E1, 0x3081}, // メ -> め
        {0x30E2, 0x3082}, // モ -> も
        {0x30E4, 0x3084}, // ヤ -> や
        {0x30E6, 0x3086}, // ユ -> ゆ
        {0x30E8, 0x3088}, // ヨ -> よ
        {0x30E9, 0x3089}, // ラ -> ら
        {0x30EA, 0x308A}, // リ -> り
        {0x30EB, 0x308B}, // ル -> る
        {0x30EC, 0x308C}, // レ -> れ
        {0x30ED, 0x308D}, // ロ -> ろ
        {0x30EF, 0x308F}, // ワ -> わ
        {0x30F2, 0x3092}, // ヲ -> を
        {0x30F3, 0x3093}, // ン -> ん
        {0x30AC, 0x304C}, // ガ -> が
        {0x30AE, 0x304E}, // ギ -> ぎ
        {0x30B0, 0x3050}, // グ -> ぐ
        {0x30B2, 0x3052}, // ゲ -> げ
        {0x30B4, 0x3054}, // ゴ -> ご
        {0x30B6, 0x3056}, // ザ -> ざ
        {0x30B8, 0x3058}, // ジ -> じ
        {0x30BA, 0x305A}, // ズ -> ず
        {0x30BC, 0x305C}, // ゼ -> ぜ
        {0x30BE, 0x305E}, // ゾ -> ぞ
        {0x30C0, 0x3060}, // ダ -> だ
        {0x30C2, 0x3062}, // ヂ -> ぢ
        {0x30C5, 0x3065}, // ヅ -> づ
        {0x30C7, 0x3067}, // デ -> で
        {0x30C9, 0x3069}, // ド -> ど
        {0x30D0, 0x3070}, // バ -> ば
        {0x30D3, 0x3073}, // ビ -> び
        {0x30D6, 0x3076}, // ブ -> ぶ
        {0x30D9, 0x3079}, // ベ -> べ
        {0x30DC, 0x307C}, // ボ -> ぼ
        {0x30D1, 0x3071}, // パ -> ぱ
        {0x30D4, 0x3074}, // ピ -> ぴ
        {0x30D7, 0x3077}, // プ -> ぷ
        {0x30DA, 0x307A}, // ペ -> ぺ
        {0x30DD, 0x307D}, // ポ -> ぽ
        {0x30E3, 0x3083}, // ャ -> ゃ
        {0x30E5, 0x3085}, // ュ -> ゅ
        {0x30E7, 0x3087}, // ョ -> ょ
        {0x30C3, 0x3063}, // ッ -> っ
        {0x30EE, 0x308E}, // ヮ -> ゎ
        {0x30F0, 0x3090}, // ヰ -> ゐ
        {0x30F1, 0x3091}  // ヱ -> ゑ
    };

    std::string hiraganaToKatakana(const std::string_view text)
    {
        if (text.empty())
            return {};

        const std::u32string utf32Text = KanjiUtils::utf8ToUtf32(text);
        std::u32string result;

        for (const char32_t ch : utf32Text)
        {
            if (auto it = HIRAGANA_TO_KATAKANA.find(ch); it != HIRAGANA_TO_KATAKANA.end())
                result.push_back(it->second);
            else
                result.push_back(ch);
        }

        // return utf8
        return KanjiUtils::utf32ToUtf8(result);
    }

    std::string katakanaToHiragana(const std::string_view text)
    {
        if (text.empty())
            return {};

        const std::u32string utf32Text = KanjiUtils::utf8ToUtf32(text);
        std::u32string result;

        for (const char32_t ch : utf32Text)
        {
            if (auto it = KATAKANA_TO_HIRAGANA.find(ch); it != KATAKANA_TO_HIRAGANA.end())
                result.push_back(it->second);
            else
                result.push_back(ch);
        }

        // return utf8
        return KanjiUtils::utf32ToUtf8(result);
    }
}