#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "yomitan_dictionary_builder/core/base_parser.h"
#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/index/index_reader.h"

#include <string>
#include <unordered_set>
#include <vector>

const std::unordered_set<std::string_view> ignoredAttributes = {
    "rel", "http-equiv", "xmlns", "hmhtml", "content", "media", "alt", "rowspan"
};

class XMLParser : public BaseParser
{
public:

    explicit XMLParser(const ParserConfig& config);

protected:

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

    /**
     * Recursively converts XML structure to Yomitan compatible format
     * @param node The XML node
     * @param ignoreExpressions Whether to ignore elements specified as expression elements
     * @return Shared to the HTML element
     */
    [[nodiscard]] std::shared_ptr<HTMLElement> convertElementToYomitan(const pugi::xml_node& node, bool ignoreExpressions = false) const;

    /**
     * Gets the stripped text contents of an XML element
     * @param node The XML node to strip
     * @param ignoredElements Set of tag names to ignore
     * @return String with the stripped text content
     */
    static std::string getElementText(const pugi::xml_node& node, const std::optional<std::set<std::string>>& ignoredElements = std::nullopt);

    std::unique_ptr<IndexReader> indexReader;

private:
    /**
     * Loads the tag mapping for a dictionary from a specified path
     * @param filePath The path to the json file
     */
    void loadTagMapping(const std::filesystem::path& filePath);

    std::unordered_map<std::string, std::string> tagMapping;
    bool hasParentSelectors = false;
};


#endif
