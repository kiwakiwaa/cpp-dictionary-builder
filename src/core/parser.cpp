#include "yomitan_dictionary_builder/core/parser.h"
#include "yomitan_dictionary_builder/core/dictionary/dicentry.h"

#include <chrono>
#include <iostream>
#include <utility>

#include "yomitan_dictionary_builder/utils/jptools/kana_convert.h"
#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

Parser::Parser(std::unique_ptr<YomitanDictionary> dictionary, const ParserConfig& parserConfig) : XMLParser(parserConfig)
{
    this->fileIterator = std::make_unique<FileUtils::FileIterator>(config.dictionaryPath);
    this->batchSize = config.parsingBatchSize;
    this->dictionary = std::move(dictionary);
    this->indexReader = config.indexPath.has_value() ? std::make_unique<IndexReader>(config.indexPath.value().string()) : nullptr;
}


Parser::~Parser() = default;


bool Parser::parseEntry(
    const std::string& term,
    const std::string& reading,
    const pugi::xml_document& doc,
    const std::optional<std::string>& infoTag,
    const std::optional<std::string>& posTag,
    const std::optional<int> searchRank,
    const std::optional<long> sequenceNumber,
    const std::optional<bool> ignoreExpressions) const
{
    auto entry = std::make_unique<DicEntry>(term, reading);

    if (infoTag && !infoTag.value().empty())
    {
        entry->setInfoTag(infoTag.value());
    }

    if (posTag && !posTag.value().empty())
    {
        entry->setPosTag(posTag.value());
    }

    if (searchRank.has_value())
    {
        entry->setSearchRank(searchRank.value());
    }

    if (sequenceNumber.has_value())
    {
        entry->setSequenceNumber(sequenceNumber.value());
    }

    const pugi::xml_node root = doc.document_element();
    if (!root)
    {
        std::cerr << "XML has no root" << std::endl;
    }

    const auto xmlTree = convertElementToYomitan(root);
    if (!xmlTree)
    {
        std::cerr << "Failed to parse xml" << std::endl;
        return false;
    }

    entry->addElement(xmlTree);
    if (!dictionary->addEntry(entry))
    {
        std::cerr << "Failed to add entry '" << term << "' to dictionary\n" << std::endl;
        return false;
    }

    return true;
}


int Parser::parse()
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

    return entriesProcessed;
}


bool Parser::exportDictionary(const std::string_view outputPath) const
{
    if (dictionary->exportDictionary(outputPath))
    {
        std::cout << "successfully exported dictionary to " << outputPath << std::endl;
        return true;
    }

    std::cerr << "Failed to export dictionary to " << outputPath << std::endl;
    return false;
}


std::vector<std::string> Parser::normalizeKeys(const std::vector<std::string>& keys, const std::string_view context)
{
    std::vector<std::string> normalizedKeys;
    if (KanjiUtils::isKanjiString(context))
    {
        for (const auto& key : keys)
        {
            normalizedKeys.emplace_back(KanaConvert::katakanaToHiragana(key));
        }
    }
    else if (KanjiUtils::containsKatakana(context))
    {
        for (const auto& key : keys)
        {
            normalizedKeys.emplace_back(KanaConvert::hiraganaToKatakana(key));
        }
    }
    else
    {
        for (const auto& key : keys)
        {
            normalizedKeys.emplace_back(KanaConvert::katakanaToHiragana(key));
        }
    }
    return normalizedKeys;
}


void Parser::updateProgress() const
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


int Parser::processBatch(const std::vector<std::filesystem::path>& filePaths)
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


