#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"

#include <iostream>

YomitanDictionary::YomitanDictionary(const std::string_view dictionaryName)
    : YomitanDictionary(YomitanDictionaryConfig{.title = std::string(dictionaryName)})
{
}

YomitanDictionary::YomitanDictionary(const YomitanDictionaryConfig &config) : config(config)
{
    if (config.tempDir.has_value())
        tempDir = config.tempDir.value();
    else
        tempDir =  getDefaultTempDir();

    ensureTempDirExits();
}

YomitanDictionary::~YomitanDictionary()
{
    try
    {
        if (currentChunk.empty())
            flush();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error when calling destructor: " << e.what() << std::endl;
    }
}

bool YomitanDictionary::addEntry(std::unique_ptr<DicEntry>& entry)
{
    try
    {
        if (!entry)
        {
            std::cerr << "Entry cannot be null" << std::endl;
            return false;
        }

        currentChunk.emplace_back(std::move(entry));
        totalEntries++;

        // Check if we should flush the current entry to disk
        if (currentChunk.size() >= config.CHUNK_SIZE)
        {
            return flushChunkToDisk();
        }

        return true;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid argument when adding entry: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error when adding entry: " << e.what() << std::endl;
        return false;
    }
}

bool YomitanDictionary::addEntry(std::unique_ptr<DicEntry> &&entry)
{
    try
    {
        if (!entry)
        {
            std::cerr << "Entry cannot be null" << std::endl;
            return false;
        }

        currentChunk.emplace_back(std::move(entry));
        totalEntries++;

        if (currentChunk.size() >= config.CHUNK_SIZE)
        {
            return flushChunkToDisk();
        }

        return true;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid argument when adding entry: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error when adding entry: " << e.what() << std::endl;
        return false;
    }
}

bool YomitanDictionary::flush()
{
    return flushChunkToDisk();
}

bool YomitanDictionary::flushChunkToDisk()
{
    if (currentChunk.empty())
        return true;

    try
    {
        if (!ensureTempDirExits())
            return false;

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
        {
            std::cerr << "Could not open term bank file: " << termBankPath << std::endl;
            return false;
        }

        std::string termBankJson;
        if (const auto ec = glz::write_json(currentChunk, termBankJson); ec)
        {
            std::cerr << "Error: when writing chunk to json: " << glz::format_error(ec, termBankJson) << std::endl;
            return false;
        }

        if (config.formatPretty)
            termBankFile << glz::prettify_json(termBankJson);
        else
            termBankFile << glz::minify_json(termBankJson);

        // Clear the chunk after write
        currentChunk.clear();
        return true;
    }
    catch (std::filesystem::filesystem_error& e)
    {
        std::cerr << "Filesystem error when flushing remaining entries: " << e.what() << e.path1() << e.code().value() << e.code().message() << std::endl;
        return false;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error when flushing remaining entries: " << e.what() << std::endl;
        return false;
    }
}

bool YomitanDictionary::exportIndex(const std::string_view outputPath) const
{
    try
    {
        const std::filesystem::path indexFilePath {(std::filesystem::path(outputPath).parent_path() / "index.json").string()};
        std::ofstream indexFile{indexFilePath, std::ios::trunc};
        if (!indexFile.is_open())
        {
            std::cerr << "Failed to open 'index.json' for writing at: " + indexFilePath.string() << std::endl;
            return false;
        }

        DictionaryIndex index{
            config.title,
            config.author.empty() ? FileUtils::getUsernameFolder() : config.author,
            config.url,
            config.description,
            config.attribution,
            config.format,
            config.revision,
        };

        std::string indexJson;
        if (const auto ec = glz::write_json(index, indexJson); ec)
        {
            std::cerr << "Error exporting index: " << glz::format_error(ec, indexJson) << std::endl;
            return false;
        }

        indexFile << glz::prettify_json(indexJson);
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error exporting index: " << e.what() << e.path1() << e.code().value() << e.code().message() << std::endl;
        return false;
    }
}

bool YomitanDictionary::ensureTempDirExits() const
{
    try
    {
        if (!std::filesystem::exists(tempDir))
        {
            if (!std::filesystem::create_directories(tempDir))
            {
                std::cerr << "Failed to create temporary directory: " << tempDir.string() << std::endl;
                return false;
            }
        }
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error when checking if temp dir exists" << e.what() << std::endl;
        return false;
    }
}

bool YomitanDictionary::moveTermBanksToOutput(const std::string_view outputPath) const
{
    try
    {
        // create output dir if it doesn't exist
        auto const outputDir = std::filesystem::path(outputPath);
        if (!std::filesystem::exists(outputDir))
        {
            if (!std::filesystem::create_directories(outputDir))
            {
                std::cerr << "Failed to create output directory: " << outputDir.string() << std::endl;
                return false;
            }
        }

        // copy all term banks to the output dir
        int copiedFiles = 0;
        for (const auto& entry : std::filesystem::directory_iterator(tempDir))
        {
            if (entry.is_regular_file() &&
                entry.path().filename().string().find("term_bank_") == 0)
            {
                std::filesystem::path destination = outputDir / entry.path().filename();
                std::filesystem::copy_file(
                    entry.path(),
                    destination,
                    std::filesystem::copy_options::overwrite_existing
                    );
                copiedFiles++;
            }
        }

        if (copiedFiles == 0)
        {
            std::cerr << "No term banks found to copy: " << outputDir.string() << std::endl;
        }
        else
        {
            std::cout << "Copied " << copiedFiles << " term banks to " << outputDir << std::endl;
        }
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Filesystem error when moving term banks to output dir: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error when moving term banks to output dir: " << e.what() << std::endl;
        return false;
    }
}

size_t YomitanDictionary::getEntryCount() const
{
    return totalEntries;
}

bool YomitanDictionary::exportDictionary(const std::string_view outputPath, bool moveTermBanks)
{
    // first flush any remaining entries
    if (!currentChunk.empty() && !flushChunkToDisk())
    {
        std::cerr << "Failed to flush remaining entries during exporting: " << std::endl;
        return false;
    }

    // export index file
    if (!exportIndex(outputPath))
    {
        return false;
    }

    // move termbanks if requested
    if (moveTermBanks && !moveTermBanksToOutput(outputPath))
    {
        return false;
    }

    return true;
}

std::filesystem::path YomitanDictionary::getDefaultTempDir()
{
    //return std::filesystem::temp_directory_path() / "yomitan-dictionary-temp";
    return std::filesystem::path{"/Users/caoimhe/Downloads/yomitan-dictionary-temp"};
}

const YomitanDictionaryConfig &YomitanDictionary::getConfig() const
{
    return config;
}


