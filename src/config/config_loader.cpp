#include "yomitan_dictionary_builder/config/config_loader.h"
#include "yomitan_dictionary_builder/config/strategy_factory.h"
#include <stdexcept>

ConfigLoader ConfigLoader::loadFromFile(const std::filesystem::path& filePath)
{
    ConfigLoader loader;
    loader.basePath_ = std::filesystem::current_path().parent_path();

    try
    {
        YAML::Node config = YAML::LoadFile((loader.basePath_ / filePath).string());

        if (config["general"])
        {
            loader.loadGeneralConfig(config["general"]);
        }

        if (config["dictionaries"])
        {
            loader.loadDictionaries(config["dictionaries"]);
        }

    }
    catch (const YAML::Exception& e)
    {
        throw std::runtime_error("YAML parsing error: " + std::string(e.what()));
    }

    return loader;
}

void ConfigLoader::loadGeneralConfig(const YAML::Node& generalNode)
{
    if (generalNode["author"])
    {
        const std::string author = generalNode["author"].as<std::string>();
        defaultYomitanConfig_.author = author;
        defaultMDictConfig_.author = author;
    }

    if (generalNode["showProgress"])
    {
        defaultParserConfig_.showProgress = generalNode["showProgress"].as<bool>();
    }

    if (generalNode["parsingBatchSize"])
    {
        defaultParserConfig_.parsingBatchSize = generalNode["parsingBatchSize"].as<int>();
    }
}

void ConfigLoader::loadDictionaries(const YAML::Node& dictionariesNode)
{
    for (const auto& dictPair : dictionariesNode)
    {
        const std::string dictName = dictPair.first.as<std::string>();
        DictionaryConfigPair config = parseDictionaryConfig(dictPair.second);
        config.parserConfig.dictionaryType = dictName;
        dictionaries_[dictName] = std::move(config);
    }
}

DictionaryConfigPair ConfigLoader::parseDictionaryConfig(const YAML::Node& dictNode) const
{
    DictionaryConfigPair config;

    // Start with defaults
    config.yomitanConfig = defaultYomitanConfig_;
    config.mDictConfig = defaultMDictConfig_;
    config.parserConfig = defaultParserConfig_;

    // Override with specific configs
    if (dictNode["YomitanDictionaryConfig"])
    {
        const auto yomitanConfig = parseYomitanConfig(dictNode["YomitanDictionaryConfig"]);
        if (!yomitanConfig.title.empty()) config.yomitanConfig.title = yomitanConfig.title;
        if (!yomitanConfig.author.empty()) config.yomitanConfig.author = yomitanConfig.author;
        if (!yomitanConfig.url.empty()) config.yomitanConfig.url = yomitanConfig.url;
        if (!yomitanConfig.description.empty()) config.yomitanConfig.description = yomitanConfig.description;
        if (!yomitanConfig.attribution.empty()) config.yomitanConfig.attribution = yomitanConfig.attribution;
        if (!yomitanConfig.revision.empty()) config.yomitanConfig.revision = yomitanConfig.revision;
        if (!yomitanConfig.CHUNK_SIZE) config.yomitanConfig.CHUNK_SIZE = yomitanConfig.CHUNK_SIZE;
    }

    if (dictNode["MDictConfig"])
    {
        const auto mdictConfig = parseMDictConfig(dictNode["MDictConfig"]);
        // Merge with defaults
        if (!mdictConfig.title.empty()) config.mDictConfig.title = mdictConfig.title;
        if (!mdictConfig.author.empty()) config.mDictConfig.author = mdictConfig.author;
        if (!mdictConfig.appendixLinkIdentifier.empty()) config.mDictConfig.appendixLinkIdentifier = mdictConfig.appendixLinkIdentifier;
        if (!mdictConfig.subElement.empty()) config.mDictConfig.subElement = mdictConfig.subElement;
    }

    if (dictNode["ParserConfig"])
    {
        const auto parserConfig = parseParserConfig(dictNode["ParserConfig"]);
        // Merge with defaults - this is more complex due to optionals
        config.parserConfig = parserConfig; // For now, simple override
    }

    return config;
}

YomitanDictionaryConfig ConfigLoader::parseYomitanConfig(const YAML::Node& node)
{
    YomitanDictionaryConfig config = {};

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

    return config;
}

MDictConfig ConfigLoader::parseMDictConfig(const YAML::Node& node)
{
    MDictConfig config;

    if (node["title"]) config.title = node["title"].as<std::string>();
    if (node["author"]) config.author = node["author"].as<std::string>();
    if (node["appendixLinkIdentifier"]) config.appendixLinkIdentifier = node["appendixLinkIdentifier"].as<std::string>();
    if (node["subElement"]) config.subElement = node["subElement"].as<std::string>();

    return config;
}

ParserConfig ConfigLoader::parseParserConfig(const YAML::Node& node) const
{
    ParserConfig config = defaultParserConfig_; // Start with defaults

    if (node["mDictLinkStrategyType"])
    {
        config.linkStrategyType = node["mDictLinkStrategyType"].as<std::string>();
        config.createMDictLinkStrategy = [strategyType = config.linkStrategyType](const MDictConfig& mDictConfig) {
            return MDictLinkStrategyFactory::getInstance().create(strategyType, mDictConfig);
        };
    }
    else //default
    {
        config.createMDictLinkStrategy = [](const MDictConfig& mDictConfig) {
            return MDictLinkStrategyFactory::getInstance().create("mdict", mDictConfig);
        };
    }

    if (node["keyExtractionStrategy"])
    {
        const std::string strategyType = node["keyExtractionStrategy"].as<std::string>();
        config.createKeyExtractionStrategy = [strategyType]() {
            return KeyExtractionStrategyFactory::getInstance().create(strategyType);
        };
    }
    else
    {
        config.createKeyExtractionStrategy = []() {
            return KeyExtractionStrategyFactory::getInstance().create("default");
        };
    }

    // Paths
    if (node["dictionaryPath"]) config.dictionaryPath = resolvePath(node["dictionaryPath"].as<std::string>());
    if (node["tagMappingPath"]) config.tagMappingPath = resolvePath(node["tagMappingPath"].as<std::string>());
    if (node["indexPath"]) config.indexPath = resolvePath(node["indexPath"].as<std::string>());
    if (node["jmdictPath"]) config.jmdictPath = resolvePath(node["jmdictPath"].as<std::string>());
    if (node["audioPath"]) config.audioPath = resolvePath(node["audioPath"].as<std::string>());
    if (node["appendixPath"]) config.appendixPath = resolvePath(node["appendixPath"].as<std::string>());
    if (node["outputPath"]) config.outputPath = resolvePath(node["outputPath"].as<std::string>());
    if (node["assetDirectory"]) config.assetDirectory = resolvePath(node["assetDirectory"].as<std::string>());
    if (node["cssDirectory"]) config.cssDirectory = resolvePath(node["cssDirectory"].as<std::string>());
    if (node["descriptionPath"]) config.descriptionPath = resolvePath(node["descriptionPath"].as<std::string>());
    if (node["iconPath"]) config.iconPath = resolvePath(node["iconPath"].as<std::string>());

    // Optional features
    if (node["ignoredElements"])
    {
        std::set<std::string> elements;
        for (const auto& elem : node["ignoredElements"])
        {
            elements.insert(elem.as<std::string>());
        }
        config.ignoredElements = elements;
    }

    if (node["expressionElement"]) config.expressionElement = node["expressionElement"].as<std::string>();
    if (node["parseAllLinks"]) config.parseAllLinks = node["parseAllLinks"].as<bool>();
    if (node["showProgress"]) config.showProgress = node["showProgress"].as<bool>();
    if (node["parsingBatchSize"]) config.parsingBatchSize = node["parsingBatchSize"].as<int>();

    return config;
}

std::filesystem::path ConfigLoader::resolvePath(const std::string& path) const
{
    return basePath_ / path;
}

DictionaryConfigPair ConfigLoader::getDictionaryConfig(const std::string& dictName) const
{
    const auto it = dictionaries_.find(dictName);
    if (it == dictionaries_.end())
    {
        throw std::runtime_error("Dictionary not found: " + dictName);
    }
    return it->second;
}

std::vector<std::string> ConfigLoader::getAvailableDictionaries() const
{
    std::vector<std::string> names;
    names.reserve(dictionaries_.size());
    for (const auto &name: dictionaries_ | std::views::keys)
    {
        names.push_back(name);
    }
    return names;
}