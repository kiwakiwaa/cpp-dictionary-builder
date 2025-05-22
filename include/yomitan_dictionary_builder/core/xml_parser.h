#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"
#include "indicators.h"

#include <string>
#include <unordered_set>
#include <vector>

const std::unordered_set<std::string> ignoredAttributes = {
    "rel", "http-equiv", "xmlns", "hmhtml", "content", "media", "alt", "rowspan"
};

class XMLParser
{
public:

    explicit XMLParser(const ParserConfig& config);

    virtual ~XMLParser() = default;

    /**
     *
     * @param tagName The name of the current node
     * @param classList The class list of the current node
     * @param parent The parent to the current node
     * @param recursionDepth Current recursion depth
     * @return Target tag from tag mapping if found, otherwise 'span'
     */
    [[nodiscard]] std::string getTargetTag(
        const std::string& tagName,
        const std::optional<std::vector<std::string>>& classList = std::nullopt,
        const std::optional<pugi::xml_node>& parent = std::nullopt,
        std::optional<int> recursionDepth = std::nullopt
    ) const;

    /**
     * Gets the class list for an XML element
     * @param node The XML element node
     * @return Vector of classes
     */
    static std::optional<std::vector<std::string>> getClassList(const pugi::xml_node& node);

    /**
     * Gets the data attributes for an XML element
     * @param node The XMl element node
     * @return Map of data string attributes
     */
    static std::unordered_map<std::string, std::string> getAttributeData(const pugi::xml_node& node);

    //HTMLElement& handleLinkElement(/*elementTag, list of elements, class list, data attributes*/);
    //HTMLElement& handleImageElement(/*elementTag, list of elements, class list, data attributes*/);

    /**
     * Recursively converts XML structure to Yomitan compatible format
     * @param node The XML node
     * @param ignoreExpressions Whether to ignore elements specified as expression elements
     * @return Shared to the HTML element
     */
     [[nodiscard]] std::shared_ptr<HTMLElement> convertElementToYomitan(const pugi::xml_node& node, bool ignoreExpressions = false) const;

protected:
    std::unique_ptr<indicators::ProgressBar> pbar;
    ParserConfig config;
    std::unordered_map<std::string, std::string> tagMapping;
};


#endif
