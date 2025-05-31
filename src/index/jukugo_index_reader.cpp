#include "yomitan_dictionary_builder/index/jukugo_index_reader.h"
#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

#include <iostream>
#include <filesystem>
#include <fstream>

JukugoIndexReader::JukugoIndexReader(const std::string_view indexPath) : indexPath(indexPath)
{
    if (!loadIndex())
    {
        std::cerr << "Failed to load index at: " << indexPath << std::endl;
    }
}


bool JukugoIndexReader::loadIndex()
{
    try
    {
        if (!std::filesystem::exists(indexPath))
        {
            std::cerr << "Index file not found: " << indexPath << std::endl;
            return false;
        }

        groupedEntries.clear();

        std::ifstream indexFile(indexPath);
        if (!indexFile.is_open())
        {
            std::cerr << "Failed to open index file: " << indexPath << std::endl;
            return false;
        }

        std::string line;
        size_t lineCount = 0;
        while (std::getline(indexFile, line))
        {
            std::vector<std::string> parts;
            size_t start = 0;
            size_t end = line.find('\t');

            while (end != std::string::npos)
            {
                parts.emplace_back(line.substr(start, end - start));
                start = end + 1;
                end = line.find('\t', start);
            }

            parts.emplace_back(line.substr(start));

            if (parts.size() < 2)
            {
                std::cerr << "Found a malformed line: " << line << std::endl;
                continue;
            }

            const std::string& key = parts[0];
            try
            {
                for (size_t i = 1; i < parts.size(); ++i)
                {
                    if (const std::string& pageNumber = parts[i]; !pageNumber.empty())
                    {
                        constexpr size_t item_start = 0;
                        const size_t item_end = pageNumber.find('-');

                        int pageID = std::stoi(pageNumber.substr(item_start, item_end - item_start));
                        int itemID = std::stoi(pageNumber.substr(item_end + 1));

                        groupedEntries[pageID][itemID].emplace_back(key);
                    }
                }
                lineCount++;
            }
            catch (std::exception& e)
            {
                std::cerr << "Invalid line: " << line << " - " << e.what() << std::endl;
            }
        }

        return true;
    }
    catch (std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}


const std::unordered_map<int, std::vector<std::string>>& JukugoIndexReader::getGroupedEntriesForPage(const int page)
{
    if (const auto it = groupedEntries.find(page); it != groupedEntries.end())
    {
        return it->second;
    }
    static const std::unordered_map<int, std::vector<std::string>> emptyMap;
    return emptyMap;
}

