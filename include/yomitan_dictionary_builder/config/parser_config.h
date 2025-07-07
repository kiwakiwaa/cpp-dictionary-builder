#ifndef PARSER_CONFIG_H
#define PARSER_CONFIG_H

#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"

class LinkHandlingStrategy;
class MDictLinkHandlingStrategy;
class ImageHandlingStrategy;
class KeyExtractionStrategy;

struct ParserConfig
{
    std::string dictionaryType;

    // Strategies
    std::string linkStrategyType = "default";
    std::string mdictLinkStrategy = "mdict";
    std::string imageStrategyType = "default";
    std::string keyStrategyType = "default";

    std::function<std::unique_ptr<LinkHandlingStrategy>()> createLinkStrategy;
    std::function<std::unique_ptr<MDictLinkHandlingStrategy>(const MDictConfig&)> createMDictLinkStrategy;
    std::function<std::unique_ptr<ImageHandlingStrategy>()> createImageStrategy;
    std::function<std::unique_ptr<KeyExtractionStrategy>()> createKeyExtractionStrategy;

    // Paths
    std::filesystem::path dictionaryPath;
    std::optional<std::filesystem::path> tagMappingPath;
    std::optional<std::filesystem::path> imageMappingPath;
    std::optional<std::filesystem::path> indexPath;
    std::optional<std::filesystem::path> jmdictPath;
    std::optional<std::filesystem::path> audioPath;
    std::optional<std::filesystem::path> appendixPath;
    std::optional<std::filesystem::path> outputPath;
    std::optional<std::filesystem::path> assetDirectory;
    std::optional<std::filesystem::path> fontDirectory;
    std::optional<std::filesystem::path> cssDirectory;
    std::optional<std::filesystem::path> descriptionPath;
    std::optional<std::filesystem::path> iconPath;

    // Optional features
    std::optional<std::set<std::string>> ignoredElements;
    std::optional<std::string> expressionElement;
    bool parseAllLinks = false;
    bool showProgress = false;
    int parsingBatchSize = 250;

    bool hasAssets() const
    {
        return assetDirectory.has_value() || cssDirectory.has_value();
    }
};

#endif
