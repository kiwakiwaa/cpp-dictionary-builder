#include "yomitan_dictionary_builder/config/strategy_factory.h"

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/strategies/key/kjt_extraction_strategy.h"
#include "yomitan_dictionary_builder/strategies/key/key_extraction_strategy.h"
#include "yomitan_dictionary_builder/strategies/link/mdict_link_handling_strategy.h"


void registerAllStrategies()
{
    auto& mdictLinkFactory = MDictLinkStrategyFactory::getInstance();
    mdictLinkFactory.registerStrategy("mdict", [](const MDictConfig& config) {
        return std::make_unique<MDictLinkHandlingStrategy>(config);
    });


    auto& keyFactory = KeyExtractionStrategyFactory::getInstance();
    keyFactory.registerStrategy("default", []() {
        return std::make_unique<DefaultKeyExtractionStrategy>();
    });

    keyFactory.registerStrategy("kjt", []() {
        return std::make_unique<KjtExtractionStrategy>();
    });
}

template<>
void MDictLinkStrategyFactory::ensureInitialized() const
{
    if (!initialized_)
    {
        registerAllStrategies();
        initialized_ = true;
    }
}

template<>
void KeyExtractionStrategyFactory::ensureInitialized() const
{
    if (!initialized_)
    {
        registerAllStrategies();
        initialized_ = true;
    }
}


// to ensure that factories are compiled
template class StrategyFactory<MDictLinkHandlingStrategy, const MDictConfig&>;
template class StrategyFactory<KeyExtractionStrategy>;