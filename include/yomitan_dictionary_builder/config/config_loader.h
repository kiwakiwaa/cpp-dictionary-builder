#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H
#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"

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
    static ConfigLoader loadFromFile(std::string_view filePath);

    DictionaryConfigPair getDictionaryInfo(const std::string& dictName) const;

    YomitanDictionaryConfig getYomitanDictionaryConfig(std::string_view dictName) const;

    ParserConfig getParserConfig(std::string_view dictName) const;

private:
    static std::set<std::string> parseSimpleSet(const std::string& input);

    std::unordered_map<std::string, DictionaryConfigPair> dictionaries_;
};

#endif
