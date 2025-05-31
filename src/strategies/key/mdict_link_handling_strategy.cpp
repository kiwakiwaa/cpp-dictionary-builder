#include "../../../include/yomitan_dictionary_builder/strategies/link/mdict_link_handling_strategy.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

MDictLinkHandlingStrategy::MDictLinkHandlingStrategy(const MDictConfig& dictionaryConfig) : dictionaryConfig(dictionaryConfig)
{
}

std::string MDictLinkHandlingStrategy::getNewHref(const std::string& href) const
{
    std::string newHref;
    if (!dictionaryConfig.appendixLinkIdentifier.empty() && href.find(dictionaryConfig.appendixLinkIdentifier) == 0)
    {
        newHref = getAppendixHref(href);
    }
    else if (std::regex_search(href.begin(), href.end(), subItemLinkRegex))
    {
        newHref = getSubItemHref(href);
    }
    else if (href.find(".aac") != std::string::npos)
    {
        newHref = getAudioHref(href);
    }
    else
    {
        newHref = getInternalHref(href);
    }

    return newHref;
}


std::string MDictLinkHandlingStrategy::getAudioHref(const std::string& href)
{
    std::string newHref;
    if (href.find("audio/") == std::string::npos)
        newHref = "sound://audio/" + href;
    else
        newHref = "sound://" + href;

    return newHref;
}


std::string MDictLinkHandlingStrategy::getAppendixHref(const std::string& href) const
{
    // Find the position of ".html"
    const auto htmlPos = href.find(".html");
    if (htmlPos == std::string::npos)
        return "";

    // Get the part before ".html"
    const std::string beforeHtml = href.substr(0, htmlPos);

    const std::u32string utf32Text = KanjiUtils::utf8ToUtf32(beforeHtml);
    size_t startCharIndex = std::string::npos;

    for (size_t i = 0; i < utf32Text.length(); ++i) {
        if (const char32_t ch = utf32Text[i]; KanjiUtils::isKanji(ch) || KanjiUtils::isKatakana(ch) || KanjiUtils::isHiragana(ch)) {
            startCharIndex = i;
            break;
        }
    }

    if (startCharIndex == std::string::npos)
        return "";

    const std::u32string filenameUtf32 = utf32Text.substr(startCharIndex);
    const std::string filename = KanjiUtils::utf32ToUtf8(filenameUtf32);

    return "entry://" + dictionaryConfig.title + "：" + filename;
}


std::string MDictLinkHandlingStrategy::getSubItemHref(const std::string& href)
{
    const int pageId = getPageId(href);
    const std::string itemIdStr = extractItemId(href);
    return "entry://" + std::to_string(80) +  std::to_string(pageId) + itemIdStr;
}


std::string MDictLinkHandlingStrategy::getInternalHref(const std::string& href)
{
    // Regular internal link
    if (const auto pos = href.find('-'); pos != std::string::npos
        && std::ranges::all_of(href.substr(0, pos), ::isdigit))
    {
        const std::string pageId = std::to_string(getPageId(href.substr(0, pos)));
        return "entry://" + pageId;
    }

    if (std::ranges::all_of(href, ::isdigit))
    {
        const std::string pageId = std::to_string(getPageId(href));
        return "entry://" + pageId;
    }

    return "";
}


std::string MDictLinkHandlingStrategy::extractItemId(const std::string& href)
{
    try
    {
        const size_t dashPos = href.find('-');
        if (dashPos == std::string::npos)
        {
            std::cerr << "Invalid ID format (no dash): " << href << std::endl;
            return "";
        }

        const std::string itemPart = href.substr(dashPos + 1);

        // Get the last 3 characters
        if (itemPart.length() < 3)
        {
            std::cerr << "Item part too short: " << itemPart << std::endl;
            return "";
        }

        const std::string lastThree = itemPart.substr(itemPart.length() - 3);

        // Convert hex to decimal
        const int decimalValue = std::stoi(lastThree, nullptr, 16);

        // Format as 3-digit zero-padded string
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(3) << decimalValue;
        return oss.str();

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error extracting item ID from " << href << ": " << e.what() << std::endl;
        return "";
    }
}


int MDictLinkHandlingStrategy::getPageId(const std::string& href)
{
    try
    {
        if (std::smatch match; std::regex_search(href, match, pageIdRegex))
        {
            return std::stoi(match[1].str());
        }

        std::cerr << "Could not extract page ID from filename: " << href << std::endl;
        return -1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error extracting page ID from " << href << ": " << e.what() << std::endl;
        return -1;
    }
}
