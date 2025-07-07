#include "yomitan_dictionary_builder/config/strategy_factory.h"

#include "yomitan_dictionary_builder/config/parser_config.h"
#include "yomitan_dictionary_builder/strategies/key/kjt_extraction_strategy.h"
#include "yomitan_dictionary_builder/strategies/key/key_extraction_strategy.h"
#include "yomitan_dictionary_builder/strategies/link/mdict_link_handling_strategy.h"
#include "yomitan_dictionary_builder/strategies/link/nds_link_extraction_strategy.h"
#include "yomitan_dictionary_builder/strategies/image/image_handling_strategy.h"
#include "yomitan_dictionary_builder/strategies/image/hashed_image_strategy.h"


void registerAllStrategies()
{
    auto& mdictLinkFactory = MDictLinkStrategyFactory::getInstance();
    mdictLinkFactory.registerStrategy("mdict", [](const MDictConfig& config) {
        return std::make_unique<MDictLinkHandlingStrategy>(config);
    });
    mdictLinkFactory.registerStrategy("nds", [](const MDictConfig& config) {
       return std::make_unique<NDSLinkExtractionStrategy>(config);
    });

    auto& keyFactory = KeyExtractionStrategyFactory::getInstance();
    keyFactory.registerStrategy("default", []() {
        return std::make_unique<DefaultKeyExtractionStrategy>();
    });

    keyFactory.registerStrategy("kjt", []() {
        return std::make_unique<KjtExtractionStrategy>();
    });

    auto& imageFactory = ImageStrategyFactory::getInstance();
    imageFactory.registerStrategy("hash", [](const ImageStrategyParams& params) {
        if (!params.imageMapPath.has_value())
            throw std::runtime_error("Hash image strategy requires imageMapPath parameter");

        return std::make_unique<HashedImageStrategy>(params.imageMapPath.value());
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

template<>
void ImageStrategyFactory::ensureInitialized() const
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
template class StrategyFactory<ImageHandlingStrategy, const ImageStrategyParams&>;