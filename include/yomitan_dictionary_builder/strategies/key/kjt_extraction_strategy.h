#ifndef KJT_EXTRACTION_STRATEGY_H
#define KJT_EXTRACTION_STRATEGY_H

#include "yomitan_dictionary_builder/strategies/key/key_extraction_strategy.h"

class KjtExtractionStrategy : public KeyExtractionStrategy
{
public:
    std::vector<std::string> extractKeys(const pugi::xml_document &doc, const std::filesystem::path &filePath) override;

private:
    static std::vector<std::string> extractVariations(const std::string& text, const pugi::xml_node& headwordNode);

    static std::vector<std::string> extractReadingVariations(const std::string& text);

    static bool shouldAddGaiji(const pugi::xml_node& headwordNode);

    static std::vector<std::string> removeDuplicates(const std::vector<std::string>& variations);
};

#endif