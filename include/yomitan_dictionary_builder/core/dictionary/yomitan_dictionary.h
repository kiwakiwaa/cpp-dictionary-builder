
#ifndef YOMITAN_DICTIONARY_H
#define YOMITAN_DICTIONARY_H

#include "yomitan_dictionary_builder/core/dictionary/dicentry.h"

struct YomitanDictionaryConfig
{
    // 必須
    std::string title;

    // 任意
    std::string author;
    std::string url;
    std::string description;
    std::string attribution;
    std::string revision;
    int format = 3;

    // 辞書作成設定
    size_t CHUNK_SIZE = 10'000;
    bool formatPretty = true;
    std::optional<std::filesystem::path> tempDir = std::nullopt;
};

class YomitanDictionary
{
public:
    friend glz::meta<DicEntry>;

    /**
     * Simple constructor to create a dictionary with just a title
     * @param dictionaryName The title of the dictionary
     */
    explicit YomitanDictionary(std::string_view dictionaryName);

    /**
     * Creates a new Yomitan dictionary with the given configuration
     * @param config
     */
    explicit YomitanDictionary(const YomitanDictionaryConfig& config);

    /**
     * Destructor that ensures any remaining entries are flushed
     */
    ~YomitanDictionary();

    /**
     * Adds a new entry to the dictionary
     * @param entry The dictionary entry to add
     * @return True if the entry was added successfully
     */
    bool addEntry(std::unique_ptr<DicEntry>& entry);

    /**
     * Adds a new entry to the dictionary
     * @param entry The dictionary to add (rvalue reference)
     * @return True if the entry was added successfully
     */
    bool addEntry(std::unique_ptr<DicEntry>&& entry);


    /**
     * Exports the dictionary to the specified path
     * @param outputPath Path where the dictionary should be exported
     * @param moveTermBanks If true, term banks will be moved to the output path
     * @return True if export was successful
     */
    bool exportDictionary(std::string_view outputPath, bool moveTermBanks = true);

    /**
     * Flushes any remaining entries to disk
     * @return True if flush was successful
     */
    bool flush();

    /**
     * Gets the number of entries added to the dictionary
     * @return Number of entries
     */
    [[nodiscard]] size_t getEntryCount() const;

    /**
     * Gets the configuration of the dictionary
     * @return Yomitan dictioanry configuration
     */
    [[nodiscard]] const YomitanDictionaryConfig& getConfig() const;

    /**
     * Gets the title name of the dictionary
     * @return The name of the dictionary
     */
    [[nodiscard]] const std::string& getTitle() const;


private:
    // Flushes the current chunk of entries to disk
    bool flushChunkToDisk();

    // Creates and exports the index.json file
    [[nodiscard]] bool exportIndex(std::string_view outputPath) const;

    // Move term banks to the output location
    [[nodiscard]] bool moveTermBanksToOutput(std::string_view outputPath) const;

    // Creates the temporary dir if it doesn't exist
    [[nodiscard]] bool ensureTempDirExits() const;

    static std::filesystem::path getDefaultTempDir();

    YomitanDictionaryConfig config;
    std::filesystem::path tempDir;
    std::vector<std::unique_ptr<DicEntry>> currentChunk;
    size_t totalEntries = 0;
    bool tempDirIsCleaned = false;
};

struct DictionaryIndex
{
    std::string title;
    std::string author;
    std::string url;
    std::string description;
    std::string attribution;
    int format;
    std::string revision;
};

template<>
struct glz::meta<DictionaryIndex>
{
    static constexpr auto value = glz::object(
        "title", &DictionaryIndex::title,
        "author", &DictionaryIndex::author,
        "url", &DictionaryIndex::url,
        "description", &DictionaryIndex::description,
        "attribution", &DictionaryIndex::attribution,
        "format", &DictionaryIndex::format,
        "revision", &DictionaryIndex::revision
    );
};

template <>
struct glz::meta<std::vector<DicEntry>>
{
    static constexpr auto value = glz::array(
        &DicEntry::toList
    );
};


#endif
