#ifndef PARSER_H
#define PARSER_H

#include "dictionary/yomitan_dictionary.h"
#include "xml_parser.h"

class Parser : public XMLParser
{
public:
    explicit Parser(const YomitanDictionary& config, int batchSize = 250);
    ~Parser();

    std::vector<std::string> normalizeKeys(const std::vector<std::string>& keys);
    std::pair<std::string, std::string> getPartOfSpeechTags(std::string_view term);

    bool parseEntry(
        std::string_view term,
        std::string_view reading,
        /* root element here */
        std::optional<std::string_view> infoTag,
        std::optional<std::string_view> posTag,
        std::optional<int> searchRank,
        std::optional<long> sequenceNumber,
        std::optional<bool> ignoreExpressions
    );

    bool processBatch(const std::pair<std::string, std::string>& batch);

    bool exportDictionary();

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