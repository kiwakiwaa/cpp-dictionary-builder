#ifndef MDICT_EXPORTER_H
#define MDICT_EXPORTER_H

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

struct MDictEntry
{
    long pageId;
    std::vector<std::string> keys;
    std::string content;

    MDictEntry(const long id, std::vector<std::string> k, std::string c)
        : pageId(id), keys(std::move(k)), content(std::move(c)) {}
};

class MDictExporter
{
public:
    explicit MDictExporter(MDictConfig& dictionaryConfig, ParserConfig& config);
    ~MDictExporter();

    void addEntry(const MDictEntry& entry);

    void finalize();

    struct ExportStats
    {
        size_t totalEntries = 0;
        size_t totalKeys = 0;
    };

    [[nodiscard]] ExportStats exportStats() const;

private:
    void writeContentSection();

    void writeKeySection();

    void writeTitleFile() const;

    void flushBuffer();

    void runMdictConvert() const;

    MDictConfig& dictionaryConfig;
    ParserConfig& config;

    std::filesystem::path outputDirectory;
    std::filesystem::path outputTxtFile;
    std::unique_ptr<std::ofstream> outputFile;

    std::vector<MDictEntry> entries;
    std::string buffer;

    static constexpr size_t BUFFER_SIZE_LIMIT = 1 * 1024 * 1024; // 1MB
    static constexpr size_t MAX_BUFFER_SIZE = 2 * 1024 * 1024; // 2MB

    ExportStats stats;
    bool finalized = false;
};

#endif
