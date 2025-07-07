#include <algorithm>

#include "yomitan_dictionary_builder/strategies/link/nds_link_extraction_strategy.h"


NDSLinkExtractionStrategy::NDSLinkExtractionStrategy(const MDictConfig &dictionaryConfig) : MDictLinkHandlingStrategy(dictionaryConfig)
{
}


std::string NDSLinkExtractionStrategy::getNewHref(const std::string& href) const
{
    std::string newHref;
    if (std::regex_search(href.begin(), href.end(), subItemLinkRegex))
    {
        newHref = getSubItemHref(href);
    }
    else if (std::ranges::all_of(href, ::isdigit))
    {
        newHref = "entry://" + href;
    }
    else
    {
        newHref = "";
    }

    return newHref;
}