#ifndef NDS_LINK_EXTRACTION_STRATEGY_H
#define NDS_LINK_EXTRACTION_STRATEGY_H

#include "yomitan_dictionary_builder/strategies/link/mdict_link_handling_strategy.h"


class NDSLinkExtractionStrategy final : public MDictLinkHandlingStrategy
{
public:

    explicit NDSLinkExtractionStrategy(const MDictConfig& dictionaryConfig);

    /**
     * Gets the href value with correct MDict support for links to other entries
     * @param href The original href to process
     * @return MDict supported href
     */
    [[nodiscard]] std::string getNewHref(const std::string& href) const override;


};


#endif
