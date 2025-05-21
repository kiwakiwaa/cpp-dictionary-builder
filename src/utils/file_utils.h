#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <fstream>
#include <string>
#include <optional>
#include <filesystem>
#include <iostream>
#include <regex>
#include <glaze/glaze.hpp>

namespace FileUtils
{
    /**
     * @brief File system iterator to process dictionary files incerementally
     */
    class FileIterator
    {
    public:
        explicit FileIterator(const std::filesystem::path& directoryPath) : directoryPath(directoryPath)
        {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
            {
                if (entry.is_regular_file() && entry.path().extension().string() == ".xml")
                {
                    allFiles.emplace_back(entry.path().string());
                }
            }
        }

        /**
         * Get the next batch of files to process
         * @param batchSize The size of the batch
         * @return Next batch of files to process (up to batchSize)
         */
        std::vector<std::filesystem::path> getNextBatch(const size_t batchSize)
        {
            std::vector<std::filesystem::path> batch;

            const size_t endIndex = std::min(currentIndex + batchSize, allFiles.size());
            for (size_t i = currentIndex; i < endIndex; ++i)
            {
                batch.emplace_back(allFiles[i]);
            }

            currentIndex = endIndex;
            return batch;
        }

        /**
         * Check if there are more files to process
         * @return True if there are more files to process
         */
        bool hasMore() const
        {
            return currentIndex < allFiles.size();
        }

        /**
         * Get total file count in directory
         * @return Total count of files in directory
         */
        size_t getTotalFilesCount() const
        {
            return allFiles.size();
        }

    private:
        std::filesystem::path directoryPath;
        std::vector<std::filesystem::path> allFiles;
        size_t currentIndex = 0;
    };

    static std::optional<std::string> readFile(const std::filesystem::path& path, const bool parseAsJson = false, const bool prettify = false)
    {
        try
        {
            std::ifstream file(path, std::ios::in | std::ios::binary);
            if (!file.is_open())
                return std::nullopt;

            file.seekg(0, std::ios::end);
            const std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::string content(size, ' ');
            file.read(content.data(), size);

            if (file.fail() && !file.eof())
                return std::nullopt;

            if (parseAsJson)
            {
                return prettify ? glz::prettify_json(content) : glz::minify_json(content);
            }

            return content;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return std::nullopt;
        }
    }

    template<typename T>
    std::optional<T> parseJson(const std::string& jsonStr)
    {
        T result{};

        if (auto error = glz::read_json(result, jsonStr); error)
        {
            std::cerr << "JSON parsing error: " << glz::format_error(error) << std::endl;
            return std::nullopt;
        }

        return result;
    }

    inline int getNextTermBankNumber(const std::filesystem::path& folderPath)
    {
        if (!std::filesystem::is_directory(folderPath))
            return 1;

        std::vector<std::string> termBankFiles;
        for (auto const& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.path().extension() == ".json")
            {
                if (auto filename = entry.path().filename().string(); filename.contains("term_bank"))
                    termBankFiles.emplace_back(filename);
            }
        }
        return static_cast<int>(termBankFiles.size()) + 1;
    }

    inline void cleanTermBankDirectory(const std::string& dirPath)
    {
        const std::regex termBankFilePattern("term_bank_\\d+\\.json");

        for (const auto& file : std::filesystem::directory_iterator(dirPath))
        {
            if (std::filesystem::is_regular_file(file) && std::regex_match(file.path().filename().string(), termBankFilePattern))
            {
                if (std::error_code ec; !std::filesystem::remove(file.path(), ec))
                {
                    std::cerr << "Error removing file: " << file.path().string() << ec.message() << std::endl;
                }
            }
        }
    }

    inline std::string getUsernameFolder()
    {
#ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
#else
        const char* home = std::getenv("HOME");
#endif
        if (!home) return "";

        const std::filesystem::path homePath(home);
        std::string homePathStr = homePath.filename().string();
        homePathStr[0] = std::toupper(homePathStr[0]);
        return homePathStr;
    }
};

#endif