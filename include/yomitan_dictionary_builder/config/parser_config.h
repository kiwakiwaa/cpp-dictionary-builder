#ifndef PARSER_CONFIG_H
#define PARSER_CONFIG_H

#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"

class LinkHandlingStrategy;
class ImageHandlingStrategy;

struct ParserConfig
{
    std::string dictionaryType;

    // Strategies
    std::string linkStrategyType = "default";
    std::string imageStrategyType = "default";
    std::function<std::unique_ptr<LinkHandlingStrategy>()> createLinkStrategy;
    std::function<std::unique_ptr<ImageHandlingStrategy>()> createImageStrategy;

    // Paths
    std::filesystem::path dictionaryPath;
    std::optional<std::filesystem::path> tagMappingPath;
    std::optional<std::filesystem::path> indexPath;
    std::optional<std::filesystem::path> jmdictPath;
    std::optional<std::filesystem::path> audioPath;
    std::optional<std::filesystem::path> appendixPath;

    // Optional features
    std::optional<std::set<std::string>> ignoredElements;
    std::optional<std::string> expressionElement;
    bool parseAllLinks = false;
    bool showProgress = false;
    int parsingBatchSize = 250;
};

#endif
