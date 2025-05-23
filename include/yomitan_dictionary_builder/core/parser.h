#ifndef PARSER_H
#define PARSER_H

#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/core/xml_parser.h"
#include "yomitan_dictionary_builder/index/index_reader.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"

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
    [[nodiscard]] bool parseEntry(
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
    [[nodiscard]] bool exportDictionary(std::string_view outputPath) const;

protected:
    /**
     * Process a single file
     * @param filePath XML file path
     * @return Number of entries parsed from file
     */
    virtual int processFile(const std::filesystem::path& filePath) = 0;

    /**
     * Normalise reading keys
     * @param keys Keys to normalise
     * @param context Context extracted from the XML used for deciding how to normalise the keys
     * @return Normalised keys
     */
    static std::vector<std::string> normalizeKeys(const std::vector<std::string>& keys, std::string_view context);

    /**
     * Get part-of-speech tags for a term
     * @param term Term to get tags for
     * @return Pair of info tag and part-of-speech tag
     */
    std::pair<std::string, std::string> getPartOfSpeechTags(std::string_view term);

    std::unique_ptr<IndexReader> indexReader;

private:

    int processBatch(const std::vector<std::filesystem::path>& filePaths);

    std::unique_ptr<FileUtils::FileIterator> fileIterator;
    std::unique_ptr<YomitanDictionary> dictionary;

    size_t batchSize{1};

    int entriesProcessed{0};
    int filesProcessed{0};
};


#endif