#include "yomitan_dictionary_builder/core/dictionary/yomitan_dictionary.h"
#include "yomitan_dictionary_builder/config/config_loader.h"
#include "yomitan_dictionary_builder/config/parser_registry.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_parser.h"

#include <iostream>

int main()
{
    const auto configLoader = ConfigLoader::loadFromFile("resources/dictionaries.yaml");

    const auto availableDictionaries = configLoader.getAvailableDictionaries();
    std::cout << "Available dictionaries: " << std::endl;
    for (const auto& dictionary : availableDictionaries)
    {
        std::cout << dictionary << '\n';
    }
    std::cout << std::endl;

    const std::string targetDictionary = "YDP";
    const auto config = configLoader.getDictionaryConfig(targetDictionary);

    const auto parser = std::make_unique<MdictParser>(config.parserConfig, config.mDictConfig);

    parser->parse();

    return 0;
}