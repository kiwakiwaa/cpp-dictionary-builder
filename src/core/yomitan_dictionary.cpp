#include "yomitan_dictionary.h"
#include <iostream>

#include "../utils/file_utils.h"

YomitanDictionary::YomitanDictionary(const std::string_view dictionaryName) : dictionaryName(dictionaryName) {}

bool YomitanDictionary::addEntry(std::unique_ptr<DicEntry>& entry)
{
    try
    {
        entries.emplace_back(std::move(entry));
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
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

