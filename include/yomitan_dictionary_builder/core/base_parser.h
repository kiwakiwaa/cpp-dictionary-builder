#ifndef BASE_PARSER_H
#define BASE_PARSER_H

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"
#include "indicators.h"

#include <chrono>

class BaseParser
{
public:

    explicit BaseParser(const ParserConfig& config);
    virtual ~BaseParser() = default;

    /**
     * Parse all dictionary files in the configured path
     * @return Number of entries parsed
     */
    int parse();

protected:
    /**
     * Process a single file
     * @param filePath XML file path
     * @return Number of entries parsed from file
     */
    virtual int processFile(const std::filesystem::path& filePath) = 0;

    /**
     * Initialise processing before handling files
     * Override in derived classes for specific initialisation
     */
    virtual void initializeProcessing() {}

    /**
     * Finalise processing after all files are handled
     * Override in derived classes
     */
    virtual void finalizeProcessing() {}

    ParserConfig config;
    std::unique_ptr<indicators::ProgressBar> pbar;
private:

    /**
     * Parses a batch of files
     * @param filePaths A vector of files to parse
     * @return The number of entries added from the batch processing
     */
    int processBatch(const std::vector<std::filesystem::path>& filePaths);

    /**
     * Updates the progress bar with current processing statistics
     */
    void updateProgress() const;

    std::unique_ptr<FileUtils::FileIterator> fileIterator;
    std::chrono::steady_clock::time_point startTime;
    size_t batchSize{1};
    int entriesProcessed{0};
    int filesProcessed{0};
};

#endif
