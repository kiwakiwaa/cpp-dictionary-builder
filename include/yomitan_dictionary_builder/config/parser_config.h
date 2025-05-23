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

    // Setters
    void setLinkStrategyType(const std::string& type) { linkStrategyType = type; }
    void setImageStrategyType(const std::string& type) { imageStrategyType = type; }
    void setDictionaryPath(const std::string& path) { dictionaryPath = path; }
    void setTagMappingPath(const std::string& path) { tagMappingPath = path; }
    void setIndexPath(const std::string& path) { indexPath = path; }
    void setJmdictPath(const std::string& path) { jmdictPath = path; }
    void setAudioPath(const std::string& path) { audioPath = path; }
    void setAppendixPath(const std::string& path) { appendixPath = path; }
    void setIgnoredElements(const std::set<std::string>& elements) { ignoredElements = elements; }
    void setExpressionElement(const std::string& element) { expressionElement = element; }
    void setParserAllLinks(const bool b) { parseAllLinks = b; }
    void setShowProgress(const bool b) { showProgress = b; }
    void setParsingBatchSize(const int b) { parsingBatchSize = b; }
};

#endif
