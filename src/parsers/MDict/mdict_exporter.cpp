#include "yomitan_dictionary_builder/parsers/MDict/mdict_exporter.h"
#include <iostream>

#include "yomitan_dictionary_builder/utils/jptools/kana_convert.h"

MDictExporter::MDictExporter(MDictConfig& dictionaryConfig, ParserConfig& config)
    : dictionaryConfig(dictionaryConfig), config(config)
{
    try
    {
        if (config.outputPath.has_value())
        {
            outputDirectory = config.outputPath.value();
            std::filesystem::create_directories(outputDirectory);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        throw std::runtime_error("Failed to create output directory: " + config.outputPath.value().string() + " - " + e.what());
    }

    try
    {
        outputTxtFile = outputDirectory / std::filesystem::path{dictionaryConfig.title + ".txt"};
        outputFile = std::make_unique<std::ofstream>(outputTxtFile, std::ios::out | std::ios::trunc);

        if (!outputFile->is_open())
        {
            throw std::runtime_error("Failed to open output file: " + config.outputPath.value().string());
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error opening output file: " << e.what() << std::endl;
        throw;
    }
}

MDictExporter::~MDictExporter()
{
    try
    {
        if (!finalized)
        {
            finalize();
        }

        if (outputFile && outputFile->is_open())
        {
            outputFile->close();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error in MDictExporter destructor: " << e.what() << std::endl;
    }
}


void MDictExporter::addEntry(const MDictEntry &entry)
{
    if (finalized)
    {
        throw std::runtime_error("Cannot add entries after finalisation");
    }

    entries.emplace_back(entry);
    stats.totalEntries++;
    stats.totalKeys += entry.keys.size();
}


void MDictExporter::finalize()
{
    if (finalized) return;

    try
    {
        writeContentSection();

        writeKeySection();

        writeTitleFile();

        flushBuffer();

        runMdictConvert();

        finalized = true;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error during finalisation: " << e.what() << std::endl;
        throw;
    }
}


void MDictExporter::writeContentSection()
{
    for (const auto& entry: entries)
    {
        if (const size_t estimatedSize = entry.content.size() + 100; buffer.size() + estimatedSize > BUFFER_SIZE_LIMIT)
        {
            flushBuffer();
        }

        buffer += std::to_string(entry.pageId);
        buffer += '\n';
        buffer += entry.content;
        buffer += "\n</>\n";

        // Emergency flush if buffer is too large
        if (buffer.size() > MAX_BUFFER_SIZE)
        {
            flushBuffer();
        }
    }

    // Flush remaining content
    flushBuffer();
}


void MDictExporter::writeKeySection()
{
    for (const auto& entry : entries)
    {
        const auto rawKeys = entry.keys;
        const auto hiraganaKeys = KanaConvert::normalizeKeys(rawKeys, "ひらがな");
        const auto katakanaKeys = KanaConvert::normalizeKeys(rawKeys, "カタカナ");

        // Export hiragana keys
        for (const auto& key : hiraganaKeys)
        {
            if (const size_t estimatedSize = key.size() + 50; buffer.size() + estimatedSize > BUFFER_SIZE_LIMIT)
            {
                flushBuffer();
            }

            if (key.find("〓") == 0)
            {
                continue;
            }

            buffer += key;
            buffer += "\n@@@LINK=";
            buffer += std::to_string(entry.pageId);
            buffer += "\n</>\n";

            // Emergency flush if buffer is too large
            if (buffer.size() > MAX_BUFFER_SIZE)
            {
                flushBuffer();
            }
        }

        // Export katakana keys
        for (const auto& key : katakanaKeys)
        {
            if (!std::ranges::any_of(key, [](const auto& ch) { return KanjiUtils::isKatakana(ch); }) || key == "〆")
                continue;

            if (key.find("〓") == 0)
            {
                continue;
            }

            if (const size_t estimatedSize = key.size() + 50; buffer.size() + estimatedSize > BUFFER_SIZE_LIMIT)
            {
                flushBuffer();
            }

            buffer += key;
            buffer += "\n@@@LINK=";
            buffer += std::to_string(entry.pageId);
            buffer += "\n</>\n";

            // Emergency flush if buffer is too large
            if (buffer.size() > MAX_BUFFER_SIZE)
            {
                flushBuffer();
            }
        }
    }

    // Flush remaining keys
    flushBuffer();
}


void MDictExporter::writeTitleFile() const
{
    const std::filesystem::path titleFilePath = outputDirectory / "title.html";

    std::ofstream titleFile(titleFilePath, std::ios::out | std::ios::trunc);
    if (!outputFile->is_open())
    {
        std::cerr << "Failed to open title file: " << titleFilePath.string() << std::endl;
        return;
    }

    titleFile << dictionaryConfig.title;
}


void MDictExporter::flushBuffer()
{
    try
    {
        if (outputFile && !buffer.empty())
        {
            *outputFile << buffer;
            outputFile->flush();
            buffer.clear();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error during flushing: " << e.what() << std::endl;
        throw;
    }
}


void MDictExporter::runMdictConvert() const
{
    // Only run if 'mdict' command is available
    if (const std::string check = "command -v mdict > /dev/null 2>&1"; std::system(check.c_str()) == 0)
    {

        const std::string mdxCommand = "mdict --title \"" + outputDirectory.string() + "/title.html\" "
                            + "--description \"" + config.descriptionPath.value().string() + "\" "
                            + "-a \"" + outputTxtFile.string() + "\" \""
                            + outputDirectory.string() + "/" + dictionaryConfig.title + ".mdx\"";

        const std::string mddCommand = "mdict --title \"" + outputDirectory.string() + "/title.html\" "
                            + "--description \"" + config.descriptionPath.value().string() + "\" "
                            + "-a \"" + config.assetDirectory.value().string() + "\" \""
                            + outputDirectory.string() + "/" + dictionaryConfig.title + ".mdd\"";


        if (const int result = std::system(mdxCommand.c_str()); result != 0)
            std:: cerr << "mdict failed with exit code: " << result << std::endl;
        else
        {
            std::filesystem::remove(outputTxtFile);
        }

        if (const int result = std::system(mddCommand.c_str()); result != 0)
            std:: cerr << "mdict failed with exit code: " << result << std::endl;
        else
        {
            std::filesystem::remove(outputDirectory.string() + "/title.html");
        }
    }
    else
    {
        std::cerr << "mdict is not installed.\n" << std::endl;
    }
}


MDictExporter::ExportStats MDictExporter::exportStats() const
{
    return stats;
}
