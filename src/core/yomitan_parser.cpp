#include "yomitan_dictionary_builder/core/yomitan_parser.h"
#include "yomitan_dictionary_builder/core/dictionary/dicentry.h"
#include "yomitan_dictionary_builder/utils/jptools/kana_convert.h"
#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

#include <iostream>
#include <utility>


YomitanParser::YomitanParser(std::unique_ptr<YomitanDictionary> dictionary, const ParserConfig& parserConfig) : XMLParser(parserConfig)
{
    this->dictionary = std::move(dictionary);
}


YomitanParser::~YomitanParser() = default;


bool YomitanParser::parseEntry(
    const std::string& term,
    const std::string& reading,
    const pugi::xml_document& doc,
    const std::optional<std::string>& infoTag,
    const std::optional<std::string>& posTag,
    const std::optional<int> searchRank,
    const std::optional<long> sequenceNumber,
    const std::optional<bool> ignoreExpressions) const
{
    auto entry = std::make_unique<DicEntry>(term, reading);

    if (infoTag && !infoTag.value().empty())
    {
        entry->setInfoTag(infoTag.value());
    }

    if (posTag && !posTag.value().empty())
    {
        entry->setPosTag(posTag.value());
    }

    if (searchRank.has_value())
    {
        entry->setSearchRank(searchRank.value());
    }

    if (sequenceNumber.has_value())
    {
        entry->setSequenceNumber(sequenceNumber.value());
    }

    const pugi::xml_node root = doc.document_element();
    if (!root)
    {
        std::cerr << "XML has no root" << std::endl;
    }

    const auto xmlTree = convertElementToYomitan(root);
    if (!xmlTree)
    {
        std::cerr << "Failed to parse xml" << std::endl;
        return false;
    }

    entry->addElement(xmlTree);
    if (!dictionary->addEntry(entry))
    {
        std::cerr << "Failed to add entry '" << term << "' to dictionary\n" << std::endl;
        return false;
    }

    return true;
}


bool YomitanParser::exportDictionary(const std::string_view outputPath) const
{
    if (dictionary->exportDictionary(outputPath))
    {
        std::cout << "successfully exported dictionary to " << outputPath << std::endl;
        return true;
    }

    std::cerr << "Failed to export dictionary to " << outputPath << std::endl;
    return false;
}