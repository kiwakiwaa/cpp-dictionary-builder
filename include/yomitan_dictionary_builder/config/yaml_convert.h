#ifndef YAML_CONVERT_H
#define YAML_CONVERT_H

#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include "yomitan_dictionary_builder/config/parser_config.h"

#include <yaml-cpp/yaml.h>

template<>
struct YAML::convert<YomitanDictionaryConfig>
{
    static bool decode(const Node& node, YomitanDictionaryConfig& config)
    {
        config = YomitanDictionaryConfig{};

        if (node["title"]) config.title = node["title"].as<std::string>();
        if (node["author"]) config.author = node["author"].as<std::string>();
        if (node["url"]) config.url = node["url"].as<std::string>();
        if (node["description"]) config.description = node["description"].as<std::string>();
        if (node["attribution"]) config.attribution = node["attribution"].as<std::string>();
        if (node["revision"]) config.revision = node["revision"].as<std::string>();
        if (node["format"]) config.format = node["format"].as<int>();
        if (node["chunk_size"]) config.CHUNK_SIZE = node["chunk_size"].as<long>();
        if (node["formatPretty"]) config.formatPretty = node["formatPretty"].as<bool>();
        if (node["tempDir"]) config.tempDir = node["tempDir"].as<std::string>();

        return true;
    }
};

template<>
struct YAML::convert<MDictConfig>
{
    static bool decode(const Node& node, MDictConfig& config)
    {
        config = MDictConfig{};

        if (node["title"]) config.title = node["title"].as<std::string>();
        if (node["author"]) config.author = node["author"].as<std::string>();
        if (node["appendixLinkIdentifier"]) config.appendixLinkIdentifier = node["appendixLinkIdentifier"].as<std::string>();
        if (node["subElement"]) config.subElement = node["subElement"].as<std::string>();

        return true;
    }
};

template<>
struct YAML::convert<ParserConfig>
{
    static bool decode(const Node& node, ParserConfig& config)
    {
        config = ParserConfig{};

       if (node["mDictLinkStrategyType"])
           config.mdictLinkStrategy = node["mDictLinkStrategyType"].as<std::string>();

        if (node["keyExtractionStrategy"])
            config.keyStrategyType = node["keyExtractionStrategy"].as<std::string>();

        // Paths
        if (node["dictionaryPath"]) config.dictionaryPath = node["dictionaryPath"].as<std::string>();
        if (node["tagMappingPath"]) config.tagMappingPath = node["tagMappingPath"].as<std::string>();
        if (node["indexPath"]) config.indexPath = node["indexPath"].as<std::string>();
        if (node["jmdictPath"]) config.jmdictPath = node["jmdictPath"].as<std::string>();
        if (node["audioPath"]) config.audioPath = node["audioPath"].as<std::string>();
        if (node["appendixPath"]) config.appendixPath = node["appendixPath"].as<std::string>();
        if (node["outputPath"]) config.outputPath = node["outputPath"].as<std::string>();
        if (node["assetDirectory"]) config.assetDirectory = node["assetDirectory"].as<std::string>();
        if (node["cssDirectory"]) config.cssDirectory = node["cssDirectory"].as<std::string>();
        if (node["descriptionPath"]) config.descriptionPath = node["descriptionPath"].as<std::string>();
        if (node["iconPath"]) config.iconPath = node["iconPath"].as<std::string>();

        // Optional features
        if (node["ignoredElements"] && node["ignoredElements"].IsSequence())
        {
            config.ignoredElements->clear();
            for (const auto& elem : node["ignoredElements"])
            {
                config.ignoredElements->insert(elem.as<std::string>());
            }
        }

        if (node["expressionElement"]) config.expressionElement = node["expressionElement"].as<std::string>();
        if (node["parseAllLinks"]) config.parseAllLinks = node["parseAllLinks"].as<bool>();
        if (node["showProgress"]) config.showProgress = node["showProgress"].as<bool>();
        if (node["parsingBatchSize"]) config.parsingBatchSize = node["parsingBatchSize"].as<int>();

        return true;
    }
};


#endif