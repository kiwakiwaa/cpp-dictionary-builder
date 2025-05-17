
#ifndef YOMITAN_DICTIONARY_H
#define YOMITAN_DICTIONARY_H

#include "dicentry.h"

class YomitanDictionary
{
public:
    friend glz::meta<YomitanDictionary>;

    explicit YomitanDictionary(std::string_view dictionaryName);

    // TODO: auto flush when chunk is full, use sizeof
    bool addEntry(std::unique_ptr<DicEntry>& entry);

    // TODO: move term banks to output path and export index
    void exportDictionary(std::string_view outputPath) const;

private:
    // TODO: Flush to temporary .json
    void flushChunkToDisk() const;

    // TODO: Fix handling for paths with spaces
    void exportIndex(std::string_view outputPath) const;

    // TODO: dont keep all entries in memory, use chunks
    std::vector<std::unique_ptr<DicEntry>> entries;
    //std::vector<std::unique_ptr<DicEntry>> currentChunk;
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
