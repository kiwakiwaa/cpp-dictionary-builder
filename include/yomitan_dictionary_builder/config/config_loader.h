#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"

#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <filesystem>

struct DictionaryConfigPair {
    YomitanDictionaryConfig yomitanConfig;
    MDictConfig mDictConfig;
    ParserConfig parserConfig;
};

class ConfigLoader {
public:
    static ConfigLoader loadFromFile(const std::filesystem::path& filePath);

    DictionaryConfigPair getDictionaryConfig(const std::string& dictName) const;
    std::vector<std::string> getAvailableDictionaries() const;


private:
    ConfigLoader() = default;

    void loadGeneralConfig(const YAML::Node& generalNode);
    void loadDictionaries(const YAML::Node& dictionariesNode);
    DictionaryConfigPair parseDictionaryConfig(const YAML::Node& dictNode) const;

    static YomitanDictionaryConfig parseYomitanConfig(const YAML::Node& node);
    static MDictConfig parseMDictConfig(const YAML::Node& node);
    ParserConfig parseParserConfig(const YAML::Node& node) const;

    std::filesystem::path resolvePath(const std::string& path) const;

    std::unordered_map<std::string, DictionaryConfigPair> dictionaries_;
    std::filesystem::path basePath_;

    // Default configs from general section
    YomitanDictionaryConfig defaultYomitanConfig_;
    MDictConfig defaultMDictConfig_;
    ParserConfig defaultParserConfig_;
};


#endif