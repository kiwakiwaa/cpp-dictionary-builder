#include "yomitan_dictionary_builder/parsers/YDP/parser.h"

#include "yomitan_dictionary_builder/config/parser_registry.h"

ParserRegistry& ParserRegistry::getInstance()
{
    static ParserRegistry instance;
    return instance;
}


void ParserRegistry::registerParser(const std::string &type, ParserCreator creator)
{
    creators_[type] = std::move(creator);
}

void ParserRegistry::registerAllParsers()
{
    auto& registry = getInstance();

    registry.registerParser("YDP", [](std::unique_ptr<YomitanDictionary> dict, const ParserConfig& config) {
        return std::make_unique<YDP::Parser>(std::move(dict), config);
    });
}


std::unique_ptr<Parser> ParserRegistry::createParser(const std::string &type,
    std::unique_ptr<YomitanDictionary> dictionary,
    const ParserConfig &config)
{
    const auto it = creators_.find(type);
    if (it == creators_.end())
    {
        throw std::runtime_error("Parser type " + type + " not registered");
    }

    return it->second(std::move(dictionary), config);
}


std::vector<std::string> ParserRegistry::getAvailableTypes() const
{
    std::vector<std::string> types;
    for (const auto &key: creators_ | std::views::keys)
    {
        types.emplace_back(key);
    }
    return types;
}