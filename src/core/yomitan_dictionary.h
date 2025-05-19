
#ifndef YOMITAN_DICTIONARY_H
#define YOMITAN_DICTIONARY_H

#include "dicentry.h"

class YomitanDictionary
{
public:
    friend glz::meta<DicEntry>;

    explicit YomitanDictionary(std::string_view dictionaryName);

    bool addEntry(std::unique_ptr<DicEntry>& entry);

    // TODO: move term banks to output path and export index
    void exportDictionary(std::string_view outputPath) const;

private:
    std::filesystem::path tempDir {"/Users/caoimhe/Downloads/test-dictionary"};
    const size_t CHUNK_SIZE = 10'000;
    bool tempDirIsCleaned = false;

    void flushChunkToDisk();

    // TODO: Fix handling for paths with spaces
    void exportIndex(std::string_view outputPath) const;

    std::vector<std::unique_ptr<DicEntry>> currentChunk;
    std::string dictionaryName;
};

// TODO: Change to string_view
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
