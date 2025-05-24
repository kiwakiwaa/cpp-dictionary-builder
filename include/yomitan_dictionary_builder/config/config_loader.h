#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"

#include <unordered_map>
#include <set>
#include <string>
#include <sstream>

struct DictionaryConfigPair
{
    YomitanDictionaryConfig yomitanConfig;
    ParserConfig parserConfig;
};

struct GeneralConfig {
    std::string author;
    bool showProgress = false;
    int parsingBatchSize = 100;
};

static constexpr std::string_view TWO_SPACES{"  "};
static constexpr std::string_view FOUR_SPACES{"    "};
static constexpr std::string_view SIX_SPACES{"      "};
static constexpr std::string_view EIGHT_SPACES{"        "};

class ConfigLoader
{
public:
    /**
     * Loads the dictionary config yaml file and stores the information in a map
     * @param filePath the path to the yaml file
     * @return Config loader with the configuration for all available dictionaries
     */
    static ConfigLoader loadFromFile(std::string_view filePath);

    /**
     * Returns a pair of dictionary and parser config for a specified dictionary
     * @param dictName The name of the dictionary
     * @return DictionaryConfigPair containing both the dictionary config and the parser config
     */
    DictionaryConfigPair getDictionaryInfo(const std::string& dictName) const;

private:
    static std::set<std::string> parseSimpleSet(const std::string& input);

    std::unordered_map<std::string, DictionaryConfigPair> dictionaries_;
};

#endif
