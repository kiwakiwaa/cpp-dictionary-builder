#ifndef PARSER_H
#define PARSER_H

#include "dictionary/yomitan_dictionary.h"
#include "xml_parser.h"

class Parser : public XMLParser
{
public:
    explicit Parser(const YomitanDictionary& config, int batchSize = 250);

    //temporary simple constructor
    explicit Parser(std::string_view dictionaryName);

    ~Parser() = default;

    std::vector<std::string> normalizeKeys(const std::vector<std::string>& keys);
    std::pair<std::string, std::string> getPartOfSpeechTags(std::string_view term);

    bool parseEntry(
        const std::string& term,
        const std::string& reading,
        const pugi::xml_document& doc,
        const std::optional<std::string>& infoTag = std::nullopt,
        const std::optional<std::string>& posTag = std::nullopt,
        std::optional<int> searchRank = std::nullopt,
        std::optional<long> sequenceNumber = std::nullopt,
        std::optional<bool> ignoreExpressions = std::nullopt
    );

    bool processBatch(const std::pair<std::string, std::string>& batch);

    bool exportDictionary(const std::string_view outputPath);

protected:
    bool processFile(std::string_view filename, std::string_view xml);

private:
    std::vector<std::pair<std::string, std::string>> getBatches();

    YomitanDictionaryConfig config;
    YomitanDictionary dictionary;

    // TODO maybe not load everything at once??? :sob:
    std::unordered_map<std::string, std::string> dict_data;
};


#endif