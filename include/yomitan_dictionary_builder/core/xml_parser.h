#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "pugixml.h"

#include <string>
#include <unordered_set>
#include <vector>

const std::unordered_set<std::string> ignoredAttributes = {
    "rel", "http-equiv", "xmlns", "hmhtml", "content", "media", "alt", "rowspan"
};

class XMLParser
{
public:

    //explicit XMLParser(const YomitanDictionaryConfig& config);

    //simple xml parser constructor TODO remove later
    explicit XMLParser() = default;

    virtual ~XMLParser() = default;

    //const std::string& getTargetTag(std::string_view tagName/*, parent = ?*/, int recursionDepth = 0);
    static std::vector<std::string> getClassList(const pugi::xml_node& node);

    static std::unordered_map<std::string, std::string> getAttributeData(const pugi::xml_node& node);

    //HTMLElement& handleLinkElement(/*elementTag, list of elements, class list, data attributes*/);
    //HTMLElement& handleImageElement(/*elementTag, list of elements, class list, data attributes*/);
    static std::shared_ptr<HTMLElement> convertElementToYomitan(const pugi::xml_node& node, bool ignoreExpressions = false);

private:
    YomitanDictionaryConfig config;
};


#endif
