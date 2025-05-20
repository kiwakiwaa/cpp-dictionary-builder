#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "dictionary/html_element.h"
#include "dictionary/yomitan_dictionary.h"
#include "../../libs/pugixml.hpp" // TODO how to use thissss???

#include <string>
#include <unordered_set>
#include <vector>

class XMLParser
{
public:

    explicit XMLParser(const YomitanDictionaryConfig& config);

    ~XMLParser();

    const std::string& getTargetTag(std::string_view tagName/*, parent = ?*/, int recursionDepth = 0);
    const std::vector<std::string>& getClassList(/*elementTag*/);
    const std::unordered_map<std::string, std::string>& getAttributeData(/*elementTag*/);

    HTMLElement& handleLinkElement(/*elementTag, list of elements, class list, data attributes*/);
    HTMLElement& handleImageElement(/*elementTag, list of elements, class list, data attributes*/);
    std::optional<HTMLElement&> convertElementToYomitan(/*elementTag, */ bool ignoreExpressions = false);

private:
    std::vector<HTMLElement>& processChildElements(/*elementTag, list of elements, class list, data attributes*/);

    YomitanDictionaryConfig config;
    constexpr std::unordered_set<std::string> yomitanSupportedTags {
        "br", "ruby", "rt", "rp", "table", "thead", "tbody", "tfoot",
        "tr", "td", "th", "span", "div", "ol", "ul", "li", "details", "summary"
    };
};


#endif
