#ifndef PARSER_H
#define PARSER_H

#include "dictionary/yomitan_dictionary.h"
#include "../config/parser_config.h"
#include "xml_parser.h"
#include "../utils/file_utils.h"

class Parser : public XMLParser
{
public:
    explicit Parser(std::unique_ptr<YomitanDictionary> dictionary, const ParserConfig& parserConfig);

    ~Parser() override;

    //temporary simple constructor
    explicit Parser(std::string_view dictionaryName);


    /**
     * Parse a single dictionary entry
     * @param term The term
     * @param reading The reading of the term
     * @param doc The XML document containing the dictionary entry
     * @param infoTag Optional info tag
     * @param posTag Optional part-of-speech tag
     * @param searchRank Optional search rank
     * @param sequenceNumber Optional sequence number
     * @param ignoreExpressions
     * @return true if parsing was successful
     */
    bool parseEntry(
        const std::string& term,
        const std::string& reading,
        const pugi::xml_document& doc,
        const std::optional<std::string>& infoTag = std::nullopt,
        const std::optional<std::string>& posTag = std::nullopt,
        std::optional<int> searchRank = std::nullopt,
        std::optional<long> sequenceNumber = std::nullopt,
        std::optional<bool> ignoreExpressions = std::nullopt
    ) const;

    /**
     * Parse all dictionary files in the configured path
     * @return Number of entries parsed
     */
    int parse();

    /**
     * Export the dictionary to the specified path
     * @param outputPath Path to export the dictionary to
     * @return true if export was successful
     */
    bool exportDictionary(const std::string_view outputPath) const;

protected:
    /**
     * Process a single file
     * @param filePath XML file path
     * @return true if processing was successful
     */
    virtual bool processFile(const std::filesystem::path& filePath);

    /**
     * Normalise reading keys
     * @param keys Keys to normalise
     * @return Normalised keys
     */
    std::vector<std::string> normalizeKeys(const std::vector<std::string>& keys);

    /**
     * Get part-of-speech tags for a term
     * @param term Term to get tags for
     * @return Pair of info tag and part-of-speech tag
     */
    std::pair<std::string, std::string> getPartOfSpeechTags(std::string_view term);

private:

    int processBatch(const std::vector<std::filesystem::path>& filePaths);

    std::unique_ptr<FileUtils::FileIterator> fileIterator;

    std::unique_ptr<YomitanDictionary> dictionary;

    ParserConfig config;
    size_t batchSize{1};

    int entriesProcessed{0};
    int filesProcessed{0};
};


#endif