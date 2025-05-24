#include "yomitan_dictionary_builder/config/config_loader.h"

#include <fstream>
#include <iostream>
#include <sstream>

ConfigLoader ConfigLoader::loadFromFile(std::string_view filePath)
{
    ConfigLoader configLoader;
    const auto configPath = std::filesystem::current_path().parent_path() / filePath;

    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + std::string(filePath));
    }

    std::string line;
    std::string currentDict;
    enum class Section { NONE, GENERAL, YOMITAN_CONFIG, PARSER_CONFIG };
    auto currentSection = Section::NONE;

    // Default values from general section
    YomitanDictionaryConfig defaultYomitanConfig;
    ParserConfig defaultParserConfig;

    while (std::getline(file, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // General section (root level)
        if (line == "general:") {
            currentSection = Section::GENERAL;
            currentDict.clear();
            continue;
        }

        // Dictionaries section (root level)
        if (line == "dictionaries:") {
            currentSection = Section::NONE;
            continue;
        }

        // General section properties (2 spaces)
        if (currentSection == Section::GENERAL && line.starts_with(TWO_SPACES) && !line.starts_with(FOUR_SPACES) &&
            line.find(':') != std::string::npos)
        {
            size_t colonPos = line.find(':');
            std::string key = line.substr(2, colonPos - 2);
            std::string value = line.substr(colonPos + 2); // skip ": "

            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }

            // Handle general settings
            if (key == "author") {
                defaultYomitanConfig.author = value;
            } else if (key == "showProgress") {
                defaultParserConfig.showProgress = value == "true";
            } else if (key == "parsingBatchSize") {
                defaultParserConfig.parsingBatchSize = std::stoi(value);
            }
            continue;
        }

        // Dictionary name (2 spaces)
        if (line.starts_with(TWO_SPACES) && !line.starts_with(FOUR_SPACES) && line.find(':') != std::string::npos)
        {
            currentDict = line.substr(2, line.find(':') - 2);
            currentSection = Section::NONE;
            defaultParserConfig.dictionaryType = currentDict;

            // Initialise with default values
            configLoader.dictionaries_[currentDict] = DictionaryConfigPair{
                defaultYomitanConfig,
                defaultParserConfig
            };
            continue;
        }

        // Section headers (4 spaces)
        if (line.starts_with(FOUR_SPACES) && !line.starts_with(SIX_SPACES) &&
            line.find(':') != std::string::npos && !currentDict.empty())
        {
            std::string sectionName = line.substr(4, line.find(':') - 4);

            if (sectionName == "YomitanDictionaryConfig") {
                currentSection = Section::YOMITAN_CONFIG;
            } else if (sectionName == "ParserConfig") {
                currentSection = Section::PARSER_CONFIG;
            } else {
                currentSection = Section::NONE;
            }
            continue;
        }

        // Property values (6 spaces)
        if (line.starts_with(SIX_SPACES) && !line.starts_with(EIGHT_SPACES) &&
            line.find(':') != std::string::npos && !currentDict.empty())
        {
            size_t colonPos = line.find(':');
            std::string key = line.substr(6, colonPos - 6);
            std::string value = line.substr(colonPos + 2); // skip ": "

            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }

            // Parse based on current section
            if (currentSection == Section::YOMITAN_CONFIG) {
                auto& yomitanConfig = configLoader.dictionaries_[currentDict].yomitanConfig;

                if (key == "title")
                    yomitanConfig.title = value;
                else if (key == "author")
                    yomitanConfig.author = value;
                else if (key == "url")
                    yomitanConfig.url = value;
                else if (key == "description")
                    yomitanConfig.description = value;
                else if (key == "attribution")
                    yomitanConfig.attribution = value;
                else if (key == "revision")
                    yomitanConfig.revision = value;
                else if (key == "format")
                    yomitanConfig.format = std::stoi(value);
                else if (key == "chunk_size")
                    yomitanConfig.CHUNK_SIZE = std::stol(value);
                else if (key == "formatPretty")
                    yomitanConfig.formatPretty = value == "true";
                else if (key == "tempDir")
                    yomitanConfig.tempDir = value;
            }
            else if (currentSection == Section::PARSER_CONFIG) {
                auto& parserConfig = configLoader.dictionaries_[currentDict].parserConfig;

                if (key == "linkStrategyType")
                    parserConfig.linkStrategyType = value;
                else if (key == "imageStrategyType")
                    parserConfig.imageStrategyType = value;
                else if (key == "dictionaryPath")
                    parserConfig.dictionaryPath = std::filesystem::current_path().parent_path() / value;
                else if (key == "tagMappingPath")
                    parserConfig.tagMappingPath = std::filesystem::current_path().parent_path() / value;
                else if (key == "indexPath")
                    parserConfig.indexPath = std::filesystem::current_path().parent_path() / value;
                else if (key == "jmdictPath")
                    parserConfig.jmdictPath = std::filesystem::current_path().parent_path() / value;
                else if (key == "audioPath")
                    parserConfig.audioPath = std::filesystem::current_path().parent_path() / value;
                else if (key == "appendixPath")
                    parserConfig.appendixPath = std::filesystem::current_path().parent_path() / value;
                else if (key == "ignoredElements")
                    parserConfig.ignoredElements = parseSimpleSet(value);
                else if (key == "expressionElement")
                    parserConfig.expressionElement = value;
                else if (key == "parseAllLinks")
                    parserConfig.parseAllLinks = value == "true";
                else if (key == "showProgress")
                    parserConfig.showProgress = value == "true";
                else if (key == "parsingBatchSize")
                    parserConfig.parsingBatchSize = std::stoi(value);
            }
        }
    }
    return configLoader;
}


DictionaryConfigPair ConfigLoader::getDictionaryInfo(const std::string& dictName) const
{
    const auto it = dictionaries_.find(dictName);
    if (it == dictionaries_.end()) {
        throw std::runtime_error("Dictionary not found: " + dictName);
    }
    return it->second;
}


std::set<std::string> ConfigLoader::parseSimpleSet(const std::string& input) {
    std::set<std::string> result;

    if (input.empty() || input.front() != '{' || input.back() != '}') {
        return result;
    }

    const std::string content = input.substr(1, input.size() - 2);

    std::stringstream ss(content);
    std::string item;
    while (std::getline(ss, item, ',')) {
        std::erase_if(item, [](const char c) {
            return std::isspace(c) || c == '"';
        });

        if (!item.empty()) {
            result.insert(item);
        }
    }

    return result;
}
