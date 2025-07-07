#ifndef MDICT_LINK_HANDLING_STRATEGY_H
#define MDICT_LINK_HANDLING_STRATEGY_H

#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include <regex>

static const std::regex pageIdRegex{R"((\d+))"};
//static const std::regex subItemLinkRegex{R"((\d+)-(8)([0-9a-fA-F]+))"};
static const std::regex subItemLinkRegex{R"((\d+)-(4)([0-9a-fA-F]+))"};

class MDictLinkHandlingStrategy
{
public:
    virtual ~MDictLinkHandlingStrategy() = default;

    /**
     * Creates a MDict link handling strategy with a dictionary configuration
     * @param dictionaryConfig The MDict config
     */
    explicit MDictLinkHandlingStrategy(const MDictConfig& dictionaryConfig);

    /**
     * Gets the href value with correct MDict support for links to other entries
     * @param href The original href to process
     * @return MDict supported href
     */
    [[nodiscard]] virtual std::string getNewHref(const std::string& href) const;

    /**
     * Gets the page ID from a string (e.g., "00207-8001" -> 207)
     * @param href Full id string
     * @return The page number (without the leading zeros)
     */
    static int getPageId(const std::string& href) ;

    /**
     * Extract item ID from SubItem id attribute (e.g., "00207-8001" -> 001)
     * @param href Full item id string
     * @return Item ID number (without the leading 8)
     */
    static std::string extractItemId(const std::string& href);

protected:
    /**
     * Gets the correct href for sub item entry. Prepends '80' to avoid collisions
     * (e.g., "00207-8001" -> entry://80207001)
     * @param href Full item id string
     * @return New href string
     */
    static std::string getSubItemHref(const std::string& href);

    const MDictConfig& dictionaryConfig;

private:
    /**
     * Gets the correct href for appendix links.
     * (e.g., "index/KJT-XX-filename.html#fragment" -> entry://filename )
     * @param href Full item id string
     * @return New href string
     */
    [[nodiscard]] std::string getAppendixHref(const std::string& href) const;


    /**
     * Gets the correct href for normal internal links.
     * (e.g., "00207-C001" -> 207, "003405" -> 3405)
     * @param href Full item id string
     * @return New href string
     */
    static std::string getInternalHref(const std::string& href);


    static std::string getAudioHref(const std::string& href);
};


#endif
