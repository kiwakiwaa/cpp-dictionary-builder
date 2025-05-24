#ifndef PARSER_REGISTRY_H
#define PARSER_REGISTRY_H

#include "yomitan_dictionary_builder/core/parser.h"
#include "yomitan_dictionary_builder/config/parser_config.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

class ParserRegistry {
public:
    using ParserCreator = std::function<std::unique_ptr<Parser>(
        std::unique_ptr<YomitanDictionary>,
        const ParserConfig&
    )>;

    /**
     * Gets the instance of the parser registry
     * @return Reference to the registry
     */
    static ParserRegistry& getInstance();

    /**
     * Registers a new parser
     * @param type The type name of the parser
     * @param creator The function to create the new parser
     */
    void registerParser(const std::string& type, ParserCreator creator);

    /**
     * Registers all parsers in the registry
     */
    static void registerAllParsers();

    /**
     * Creates a new parser of specified type with a config and a yomitan dictionary
     * @param type The type name of the parser
     * @param dictionary The yomitan dictionary to parse
     * @param config The parser config
     * @return
     */
    std::unique_ptr<Parser> createParser(
        const std::string& type,
        std::unique_ptr<YomitanDictionary> dictionary,
        const ParserConfig& config
    );

    [[nodiscard]] std::vector<std::string> getAvailableTypes() const;

private:
    ParserRegistry() = default;
    std::unordered_map<std::string, ParserCreator> creators_;
};

#endif