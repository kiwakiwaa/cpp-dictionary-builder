#include "yomitan_dictionary_builder/core/base_parser.h"


BaseParser::BaseParser(const ParserConfig& config) : config(config)
{
    fileIterator = std::make_unique<FileUtils::FileIterator>(config.dictionaryPath);
    batchSize = config.parsingBatchSize;
    if (config.showProgress)
    {
        pbar = std::make_unique<indicators::ProgressBar>(
            indicators::option::BarWidth{40},
            indicators::option::Start{"【"},
            indicators::option::Fill{"█"},
            indicators::option::Lead{"█"},
            indicators::option::Remainder{"-"},
            indicators::option::End{"】"},
            indicators::option::ForegroundColor{indicators::Color::magenta},
            indicators::option::ShowPercentage{true},
            indicators::option::ShowElapsedTime{true},
            indicators::option::FontStyles{std::vector{indicators::FontStyle::bold}}
        );
    }
}


int BaseParser::parse()
{
    if (!this->fileIterator)
    {
        std::cerr << "No dictionary path set." << std::endl;
    }

    const size_t totalFiles = fileIterator->getTotalFilesCount();

    if (config.showProgress)
    {
        pbar->set_option(indicators::option::PrefixText{std::to_string(totalFiles) + std::string{"のファイルを処理中"}});
        pbar->set_progress(0.0);
    }

    startTime = std::chrono::high_resolution_clock::now();
    entriesProcessed = 0;
    filesProcessed = 0;

    initializeProcessing();

    while (fileIterator->hasMore())
    {
        auto batch = fileIterator->getNextBatch(batchSize);
        this->entriesProcessed += processBatch(batch);

        if (config.showProgress)
        {
            updateProgress();
        }
    }

    if (config.showProgress)
    {
        pbar->set_option(indicators::option::PostfixText{""});
        pbar->set_option(indicators::option::PrefixText{std::to_string(entriesProcessed) + "の項目を収録しました"});
        pbar->set_progress(100.0);
    }

    finalizeProcessing();

    return entriesProcessed;
}


int BaseParser::processBatch(const std::vector<std::filesystem::path>& filePaths)
{
    int batchEntriesProcessed = 0;

    for (const auto& filePath : filePaths)
    {
        if (const int entriesFromFile = processFile(filePath); entriesFromFile > 0)
        {
            batchEntriesProcessed += entriesFromFile;
        }

        // Update progress after each file (regardless of success/failure)
        filesProcessed++;
    }

    return batchEntriesProcessed;
}


void BaseParser::updateProgress() const
{
    if (!config.showProgress || pbar->is_completed())
    {
        return;
    }

    const size_t totalFiles = fileIterator->getTotalFilesCount();
    const double progress = 100.0 * static_cast<double>(filesProcessed) / static_cast<double>(totalFiles);

    // Calculate performance metrics
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
    const double filesPerSecond = (filesProcessed > 0 && elapsed > 0) ?
        static_cast<double>(filesProcessed) / static_cast<double>(elapsed) : 0.0;

    // Update progress bar
    const std::string postfixText = std::to_string(filesPerSecond) + " ファイル/s | 項目：" + std::to_string(entriesProcessed);
    pbar->set_option(indicators::option::PostfixText{postfixText});

    // Set progress (avoid 100% until completely done)
    if (progress >= 100.0)
    {
        pbar->set_progress(99);
    }
    else
    {
        pbar->set_progress(progress);
    }
}