#include "yomitan_dictionary_builder/strategies/key/kjt_extraction_strategy.h"
#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

#include <unordered_set>

std::vector<std::string> KjtExtractionStrategy::extractKeys(const pugi::xml_document& doc, const std::filesystem::path& filePath)
{
    std::vector<std::string> keys;

    if (const pugi::xml_node zinmeiNode = doc.select_node("//ZinmeiSyomeiHeadG").node(); zinmeiNode)
    {
        // extract headword and yomi from Zinmei element
        const pugi::xml_node headwordNode = zinmeiNode.child("headword");
        const pugi::xml_node yomiNode = zinmeiNode.child("yomi");

        if (headwordNode)
        {
            if (const std::string headword = headwordNode.text().as_string(); !headword.empty())
            {
                auto variations = extractVariations(headword, headwordNode);
                keys.insert(keys.end(), variations.begin(), variations.end());
            }
        }

        if (yomiNode)
        {
            if (const std::string yomi = yomiNode.text().as_string(); !yomi.empty())
            {
                auto variations = extractReadingVariations(yomi);
                keys.insert(keys.end(), variations.begin(), variations.end());
            }
        }
    }

    return keys;
}


std::vector<std::string> KjtExtractionStrategy::extractVariations(const std::string &text, const pugi::xml_node& headwordNode)
{
    std::vector<std::string> variations;

    // if there's gaiji, and it's not in parentheses return empty
    if (shouldAddGaiji(headwordNode))
        return {};

    std::u32string u32Text = KanjiUtils::utf8ToUtf32(text);

    // check for ・ and split into variations
    if (const size_t middleDot = u32Text.find(U'・'); middleDot != std::string::npos)
    {
        std::u32string before = u32Text.substr(0, middleDot);
        std::u32string after = u32Text.substr(middleDot + 1);

        // recursively process each variation
        std::vector<std::string> beforeVariations = extractVariations(KanjiUtils::utf32ToUtf8(before), headwordNode);
        std::vector<std::string> afterVariations = extractVariations(KanjiUtils::utf32ToUtf8(after), headwordNode);

        variations.insert(variations.end(), beforeVariations.begin(), beforeVariations.end());
        variations.insert(variations.end(), afterVariations.begin(), afterVariations.end());

        return removeDuplicates(variations);
    }

    // if no parentheses, return text
    size_t openParen = u32Text.find(U'（');
    size_t closeParen = u32Text.find(U'）');

    if (openParen == std::u32string::npos || closeParen == std::u32string::npos || closeParen < openParen)
    {
        if (const auto pos = text.find("（"); pos != std::string::npos)
            variations.emplace_back(text.substr(0, pos));
        else
            variations.emplace_back(text);

        return variations;
    }

    // first parentheses pair
    std::u32string before = u32Text.substr(0, openParen);
    std::u32string inside = u32Text.substr(openParen + 1, closeParen - openParen - 1);
    std::u32string after = u32Text.substr(closeParen + 1);

    // recursively add variations
    std::vector<std::string> remainingVariations = extractVariations(KanjiUtils::utf32ToUtf8(after), headwordNode);

    std::u32string originalForm = before + after;
    std::vector<std::string> baseVariations = extractVariations(KanjiUtils::utf32ToUtf8(originalForm), headwordNode);
    variations.insert(variations.end(), baseVariations.begin(), baseVariations.end());

    // try replacing the last n kanji
    if (before.length() >= inside.length() && inside.length() > 0)
    {
        bool allKanji = true;
        size_t startPos = before.length() - inside.length();

        for (size_t i = startPos; i < before.length(); i++)
        {
            if (!KanjiUtils::isKanji(before[i]))
            {
                allKanji = false;
                break;
            }
        }

        if (allKanji)
        {
            // for each variation, create a new variation with the replaced kanji
            for (const auto& variation : remainingVariations)
            {
                std::u32string altForm = before.substr(0, startPos) + inside + KanjiUtils::utf8ToUtf32(variation);
                variations.emplace_back(KanjiUtils::utf32ToUtf8(altForm));
            }
        }
    }

    return removeDuplicates(variations);
}


bool KjtExtractionStrategy::shouldAddGaiji(const pugi::xml_node &headwordNode)
{
    // check if there are gaiji images
    bool hasGaiji = false;
    bool gaijiInParentheses = false;

    for (auto imgNode : headwordNode.select_nodes(".//img[@class='gaiji']"))
    {
        hasGaiji = true;

        pugi::xml_node prevSibling = imgNode.node().previous_sibling();
        pugi::xml_node nextSibling = imgNode.node().next_sibling();

        // check if its within parentheses
        bool foundOpenParen = false;
        bool foundCloseParen = false;

        if (prevSibling && prevSibling.type() == pugi::node_pcdata)
        {
            if (std::string prevText = prevSibling.value(); prevText.find("（") != std::string::npos)
            {
                foundOpenParen = true;
            }
        }

        if (nextSibling && nextSibling.type() == pugi::node_pcdata)
        {
            if (std::string prevText = prevSibling.value(); prevText.find("）") != std::string::npos)
            {
                foundCloseParen = true;
            }
        }

        if (foundOpenParen && foundCloseParen)
        {
            gaijiInParentheses = true;
            break;
        }
    }

    return hasGaiji && !gaijiInParentheses;
}


std::vector<std::string> KjtExtractionStrategy::removeDuplicates(const std::vector<std::string> &variations)
{
    // remove duplicates
    std::vector<std::string> uniqueVariations;
    std::unordered_set<std::string> seen;

    for (const auto& variation : variations)
    {
        if (!seen.contains(variation))
        {
            seen.insert(variation);
            uniqueVariations.emplace_back(variation);
        }
    }
    return uniqueVariations;
}


std::vector<std::string> KjtExtractionStrategy::extractReadingVariations(const std::string &text)
{
    std::vector<std::string> variations;

    const std::u32string u32Text = KanjiUtils::utf8ToUtf32(text);

    std::u32string cleaned;
    bool inParentheses = false;

    // remove all parentheses and their content
    for (const char32_t c : u32Text)
    {
        if (c == U'（')
        {
            inParentheses = true;
            continue;
        }
        if (c == U'）')
        {
            inParentheses = false;
            continue;
        }
        if (!inParentheses && c != U' ')
        {
            cleaned.push_back(c);
        }
    }

    variations.emplace_back(KanjiUtils::utf32ToUtf8(cleaned));
    return variations;
}

