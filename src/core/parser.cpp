#include "yomitan_dictionary_builder/core/parser.h"
#include "yomitan_dictionary_builder/core/dictionary/dicentry.h"

#include <chrono>
#include <iostream>
#include <utility>

Parser::Parser(std::unique_ptr<YomitanDictionary> dictionary, ParserConfig  parserConfig): config(std::move(parserConfig))
{
    this->fileIterator = std::make_unique<FileUtils::FileIterator>(config.dictionaryPath.value());
    this->batchSize = config.parsingBatchSize;
    this->dictionary = std::move(dictionary);
    this->indexReader = config.indexPath.has_value() ? std::make_unique<IndexReader>(config.indexPath.value().string()) : nullptr;
}

Parser::Parser(std::string_view dictionaryName) : config(ParserConfig{"", ""})
{
    dictionary = std::make_unique<YomitanDictionary>(dictionaryName);
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
    if (xmlTree)
    {
        //std::cout << "successfully parsed xml" << std::endl;
        //xmlTree->print();
    }
    else
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
    std::cout << "Processing " << totalFiles << " files..." << std::endl;

    const auto startTime = std::chrono::high_resolution_clock::now();
    entriesProcessed = 0;
    filesProcessed = 0;


    while (fileIterator->hasMore())
    {
        auto batch = fileIterator->getNextBatch(batchSize);
        this->entriesProcessed += processBatch(batch);

        // Progress
        const int progress = static_cast<int>((static_cast<double>(filesProcessed) / totalFiles) * 100);
        auto currentTime = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        const double filesPerSecond = filesProcessed > 0 ? static_cast<double>(filesProcessed) / elapsed : 0;

        std::cout << "\nProgress: " << progress << "% | "
                    << filesProcessed << "/" << totalFiles << " files | "
                    << filesPerSecond << "files/s | "
                    << entriesProcessed << " entries | "
                    << "Elapsed: " << elapsed << "s | "
                    << std::flush;
    }

    std::cout << "Parsed " << entriesProcessed << " entries." << std::endl;

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


bool Parser::processFile(const std::filesystem::path& filePath)
{
    pugi::xml_document doc;
    if (const pugi::xml_parse_result result = doc.load_file(filePath.c_str()); !result)
    {
        std::cerr << "Failed to read xml: " << filePath.filename().string() << std::endl;
        return false;
    }

    const bool success = parseEntry(
        "",
        "",
        doc
    );

    if (success)
    {
        entriesProcessed++;
    }

    return success;
}



int Parser::processBatch(const std::vector<std::filesystem::path> &filePaths)
{
    int batchEntriesProcessed = 0;

    for (const auto& filePath : filePaths)
    {
        if (processFile(filePath))
        {
            batchEntriesProcessed++;
            filesProcessed++;
        }
    }

    return batchEntriesProcessed;
}


