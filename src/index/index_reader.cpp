#include "index_reader.h"
#include "../utils/jptools/kana_convert.h"

IndexReader::IndexReader(const std::string_view indexPath) : indexPath(indexPath)
{
}

const std::vector<std::string>& IndexReader::getKeysForFile(const std::string_view filename)
{
    if (const auto iter = fileToKeys.find(std::string(filename)); iter != fileToKeys.end())
    {
        return iter->second;
    }
    return emptyVector;
}

bool IndexReader::loadIndex()
{
    try
    {
        if (!std::filesystem::exists(indexPath))
        {
            std::cerr << "Index file not found: " << indexPath << std::endl;
            return false;
        }

        fileToKeys.clear();

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

            std::string key = parts[0];
            std::vector<std::string> pageNumbers;

            for (size_t i = 1; i < parts.size(); ++i)
            {
                if (std::string pageNumber = parts[i]; !pageNumber.empty())
                {
                    pageNumbers.emplace_back(pageNumber);

                    // Build reverse mapping
                    fileToKeys[pageNumber].emplace_back(key);
                }
            }
            lineCount++;
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


