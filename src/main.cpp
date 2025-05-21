#include "core/dictionary/yomitan_dictionary.h"
#include "core/parser.h"
#include "config/parser_config.h"
#include <iostream>
#include <filesystem>

class ParserFactory
{
public:
    static std::unique_ptr<Parser> createParser(std::unique_ptr<YomitanDictionary> dictionary, ParserConfig& config)
    {
        // Default to base parser
        return std::make_unique<Parser>(std::move(dictionary), config);
    }
};

int main()
{
    auto dictionary = std::make_unique<YomitanDictionary>(YomitanDictionaryConfig{
        "大漢和辞典",
        "Bint",
        "yomitan.com",
        "test dictionary",
        "only me",
        "test-dict",
        3,
        2500,
        true,
    });

     auto config = ParserConfigBuilder(
        "/Users/caoimhe/Documents/日本語/Dictionary Conversion/monokakido-to-yomitan/data/YDP/pages",
        "YDP"
    )
    .build();

    const auto parser = ParserFactory::createParser(std::move(dictionary), config);
    const int parsedEntries = parser->parse();
    std::cout << "Parsed entries: " << parsedEntries << std::endl;

    if (const auto result = parser->exportDictionary("/Users/caoimhe/Downloads/test-dictionary"))
    {
        std::cout << "Successfully exported dict to specified path" << std::endl;
    }
    return 0;
}
