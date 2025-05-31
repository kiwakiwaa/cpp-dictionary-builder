#ifndef MDICT_CONFIG_H
#define MDICT_CONFIG_H

#include "yomitan_dictionary_builder/core/asset_manager.h"
#include <string>

struct MDictConfig
{
    std::string title;
    std::string author;
    std::string appendixLinkIdentifier;
    std::string subElement;

    AssetConfig assets;
};

#endif
