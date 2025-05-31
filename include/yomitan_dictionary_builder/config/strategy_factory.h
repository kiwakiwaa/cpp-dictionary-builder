#ifndef STRATEGY_FACTORY_H
#define STRATEGY_FACTORY_H

#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include "yomitan_dictionary_builder/strategies/key/key_extraction_strategy.h"
#include "yomitan_dictionary_builder/strategies/link/link_handling_strategy.h"
#include "yomitan_dictionary_builder/strategies/link/mdict_link_handling_strategy.h"
#include "yomitan_dictionary_builder/strategies/image/image_handling_strategy.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>

class LinkHandlingStrategy;
class MDictLinkHandlingStrategy;
class ImageHandlingStrategy;
class KeyExtractionStrategy;

template<typename T, typename... Args>
class StrategyFactory
{
public:
    using CreateFunction = std::function<std::unique_ptr<T>(Args...)>;

    static StrategyFactory& getInstance()
    {
        static StrategyFactory instance;
        return instance;
    }

    void registerStrategy(const std::string& name, CreateFunction creator)
    {
        creators_[name] = std::move(creator);
    }

    std::unique_ptr<T> create(const std::string& name, Args... args) const
    {
        ensureInitialized();

        auto it = creators_.find(name);
        if (it == creators_.end())
        {
            throw std::runtime_error("Unknown strategy: " + name);
        }
        return it->second(std::forward<Args>(args)...);
    }

    bool hasStrategy(const std::string& name) const
    {
        ensureInitialized();
        return creators_.contains(name);
    }

private:
    mutable std::unordered_map<std::string, CreateFunction> creators_;
    mutable bool initialized_ = false;

    void ensureInitialized() const;
};

// Type aliases for convenience
using LinkStrategyFactory = StrategyFactory<LinkHandlingStrategy>;
using MDictLinkStrategyFactory = StrategyFactory<MDictLinkHandlingStrategy, const MDictConfig&>;
using KeyExtractionStrategyFactory = StrategyFactory<KeyExtractionStrategy>;

void registerAllStrategies();

#endif