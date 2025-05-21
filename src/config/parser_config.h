#ifndef PARSER_CONFIG_H
#define PARSER_CONFIG_H

#include "../core/dictionary/yomitan_dictionary.h"

class LinkHandlingStrategy;
class ImageHandlingStrategy;

struct ParserConfig
{
    std::string dictionaryType;

    // Strategies
    std::function<std::unique_ptr<LinkHandlingStrategy>()> createLinkStrategy;
    std::function<std::unique_ptr<ImageHandlingStrategy>()> createImageStrategy;

    // Paths
    std::optional<std::filesystem::path> dictionaryPath;
    std::optional<std::filesystem::path> tagMappingPath;
    std::optional<std::filesystem::path> indexPath;
    std::optional<std::filesystem::path> jmdictPath;
    std::optional<std::filesystem::path> audioPath;
    std::optional<std::filesystem::path> appendixPath;

    // Optional features
    std::optional<std::set<std::string>> ignoredElements;
    std::optional<std::string> expressionElement;
    bool parseAllLinks = false;

    int parsingBatchSize = 250;

    explicit ParserConfig(
        const std::filesystem::path& dictionaryPath,
        const std::string_view dictionaryType
        ) : dictionaryPath(dictionaryPath), dictionaryType(dictionaryType)
    {
    }
};


class ParserConfigBuilder {

public:

    explicit ParserConfigBuilder(
        const std::filesystem::path& dictionaryPath,
        const std::string_view dictionaryType
        ) : parserConfig(dictionaryPath, dictionaryType)
    {
    }

    ParserConfigBuilder& withDictionaryPath(const std::filesystem::path& path)
    {
        parserConfig.dictionaryPath = path;
        return *this;
    }

    ParserConfigBuilder& withTagMappingPath(const std::filesystem::path& path)
    {
        parserConfig.tagMappingPath = path;
        return *this;
    }

    ParserConfigBuilder& withIndexPath(const std::filesystem::path& path)
    {
        parserConfig.indexPath = path;
        return *this;
    }

    ParserConfigBuilder& withJmdictPath(const std::filesystem::path& path)
    {
        parserConfig.jmdictPath = path;
        return *this;
    }

    ParserConfigBuilder& withAudioPath(const std::filesystem::path& path)
    {
        parserConfig.audioPath = path;
        return *this;
    }

    ParserConfigBuilder& withAppendixPath(const std::filesystem::path& path)
    {
        parserConfig.appendixPath = path;
        return *this;
    }

    ParserConfigBuilder& withLinkStrategy(std::function<std::unique_ptr<LinkHandlingStrategy>()> linkStrategy)
    {
        parserConfig.createLinkStrategy = std::move(linkStrategy);
        return *this;
    }

    ParserConfigBuilder& withImageStrategy(std::function<std::unique_ptr<ImageHandlingStrategy>()> imageStrategy)
    {
        parserConfig.createImageStrategy = std::move(imageStrategy);
        return *this;
    }

    ParserConfigBuilder& withIgnoredElements(const std::set<std::string>& ignoredElements)
    {
        parserConfig.ignoredElements = ignoredElements;
        return *this;
    }

    ParserConfigBuilder& withExpressionElement(const std::string& expressionElement)
    {
        parserConfig.expressionElement = expressionElement;
        return *this;
    }

    ParserConfigBuilder& withParseAllLinks(const bool parseAllLinks)
    {
        parserConfig.parseAllLinks = parseAllLinks;
        return *this;
    }

    ParserConfigBuilder& withParsingBatchSize(const int parsingBatchSize)
    {
        parserConfig.parsingBatchSize = parsingBatchSize;
        return *this;
    }

    ParserConfig build()
    {
        return parserConfig;
    }

private:
    ParserConfig parserConfig;

};

#endif
