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

    static ParserRegistry& getInstance();

    void registerParser(const std::string& type, ParserCreator creator);

    static void registerAllParsers();

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