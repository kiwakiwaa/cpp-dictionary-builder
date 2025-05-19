#include "yomitan_dictionary.h"
#include <iostream>

#include "../utils/file_utils.h"

YomitanDictionary::YomitanDictionary(const std::string_view dictionaryName) : dictionaryName(dictionaryName) {}

bool YomitanDictionary::addEntry(std::unique_ptr<DicEntry>& entry)
{
    try
    {
        currentChunk.emplace_back(std::move(entry));
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    // Check if we should flush the current entry to disk
    if (currentChunk.size() >= CHUNK_SIZE)
    {
        flushChunkToDisk();
        currentChunk.clear();
    }

    return true;
}

void YomitanDictionary::flushChunkToDisk()
{
    try
    {
        if (!std::filesystem::exists(tempDir))
            std::filesystem::create_directories(tempDir);

        if (!tempDirIsCleaned)
        {
            FileUtils::cleanTermBankDirectory(tempDir.string());
            tempDirIsCleaned = true;
        }

        const auto nextTermBankNumber = FileUtils::getNextTermBankNumber(tempDir);
        const std::string_view filename {"term_bank_" + std::to_string(nextTermBankNumber) + ".json"};
        const std::filesystem::path termBankPath {tempDir / filename};

        std::ofstream termBankFile {termBankPath, std::ios::trunc};
        if (!termBankFile.is_open())
            std::cerr << "Could not open term bank file: " << termBankPath << std::endl;

        std::string termBankJson;
        if (const auto ec = glz::write_json(currentChunk, termBankJson); ec)
            std::cerr << "Error: " << glz::format_error(ec, termBankJson) << std::endl;

        termBankFile << glz::prettify_json(termBankJson);
    }
    catch (std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << e.path1() << e.code().value() << e.code().message() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void YomitanDictionary::exportIndex(const std::string_view outputPath) const
{
    try
    {
        const std::filesystem::path indexFilePath {(std::filesystem::path(outputPath).parent_path() / "index.json").string()};
        std::ofstream indexFile{indexFilePath, std::ios::trunc};
        if (!indexFile.is_open())
        {
            std::cerr << "Failed to open 'index.json' for writing at: " + indexFilePath.string() << std::endl;
        }

        DictionaryIndex index{
            dictionaryName,
            FileUtils::getUsernameFolder(),
            "",
            "",
            "",
            3,
            "",
        };

        std::string indexJson;
        if (const auto ec = glz::write_json(index, indexJson); ec)
            std::cerr << "Error: " << glz::format_error(ec, indexJson) << std::endl;

        indexFile << glz::prettify_json(indexJson);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << e.path1() << e.code().value() << e.code().message() << std::endl;
    }
}


void YomitanDictionary::exportDictionary(const std::string_view outputPath) const
{
    exportIndex(outputPath);
}

