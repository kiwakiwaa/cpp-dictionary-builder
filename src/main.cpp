#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/config/config_loader.h"
#include "yomitan_dictionary_builder/config/parser_registry.h"

#include <iostream>

int main()
{
    ParserRegistry::registerAllParsers();

    const auto configLoader = ConfigLoader::loadFromFile("resources/dictionaries.yaml");
    auto [yomitanConfig, parserConfig] = configLoader.getDictionaryInfo("YDP");
    auto dictionary = std::make_unique<YomitanDictionary>(yomitanConfig);

    auto& registry = ParserRegistry::getInstance();
    const auto parser = registry.createParser("YDP", std::move(dictionary), parserConfig);

    const int parsedEntries = parser->parse();
    std::cout << "Parsed entries: " << parsedEntries << std::endl;

    if ([[maybe_unused]] auto result = parser->exportDictionary("/Users/caoimhe/Downloads/test-dictionary"))
    {
        std::cout << "Successfully exported dict to specified path" << std::endl;
    }
    return 0;
}