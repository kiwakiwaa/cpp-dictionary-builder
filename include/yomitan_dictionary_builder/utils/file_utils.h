#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <fstream>
#include <string>
#include <optional>
#include <filesystem>
#include <iostream>
#include <regex>
#include <glaze/glaze.hpp>
#include "pugixml.h"

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
            if (!std::filesystem::exists(directoryPath))
            {
                throw std::runtime_error("Directory does not exist: " + directoryPath.string());
            }

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
        [[nodiscard]] bool hasMore() const
        {
            return currentIndex < allFiles.size();
        }

        /**
         * Get total file count in directory
         * @return Total count of files in directory
         */
        [[nodiscard]] size_t getTotalFilesCount() const
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

    inline bool loadXMLFile(pugi::xml_document& doc, const std::filesystem::path& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "File '" << filePath.string() << "' does not exist!" << std::endl;
            return false;
        }

        if (const pugi::xml_parse_result result = doc.load_file(filePath.c_str()); !result)
        {
            std::cerr << "Failed to read xml: " << filePath.filename().string() << std::endl;
            return false;
        }

        return true;
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
        std::error_code ec;
        if (!std::filesystem::is_directory(folderPath, ec) || ec) {
            return 1;
        }

        int maxNumber = 0;
        const std::regex termBankPattern(R"(term_bank_(\d+)\.json$)");

        for (const auto& entry : std::filesystem::directory_iterator(folderPath, ec)) {
            if (ec) break;

            if (!entry.is_regular_file(ec) || ec) continue;

            const std::string filename = entry.path().filename().string();
            if (std::smatch match; std::regex_match(filename, match, termBankPattern)) {
                int number = std::stoi(match[1].str());
                maxNumber = std::max(maxNumber, number);
            }
        }

        return maxNumber + 1;
    }

    inline std::optional<std::string> getUsernameFolder()
    {
#ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
#else
        const char* home = std::getenv("HOME");
#endif
        if (!home || std::strlen(home) == 0) {
            return std::nullopt;
        }

        const std::filesystem::path homePath(home);
        std::string username = homePath.filename().string();

        if (username.empty()) {
            return std::nullopt;
        }


        if (std::isalpha(static_cast<unsigned char>(username[0]))) {
            username[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(username[0])));
        }

        return username;
    }
};

#endif