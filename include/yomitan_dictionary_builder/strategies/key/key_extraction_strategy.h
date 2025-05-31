#ifndef KEY_EXTRACTION_STRATEGY_H
#define KEY_EXTRACTION_STRATEGY_H

#include <string>
#include <vector>
#include <filesystem>
#include "pugixml.h"

class KeyExtractionStrategy
{
public:
    virtual ~KeyExtractionStrategy() = default;
    virtual std::vector<std::string> extractKeys(const pugi::xml_document& doc, const std::filesystem::path& filePath) = 0;
};

class DefaultKeyExtractionStrategy : public KeyExtractionStrategy
{
public:
    std::vector<std::string> extractKeys(const pugi::xml_document &doc, const std::filesystem::path &filePath) override
    {
        // rely on index reader only
        return {};
    }
};

#endif