#ifndef SUBITEM_PROCESSOR_H
#define SUBITEM_PROCESSOR_H

#include "pugixml.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_exporter.h"

#include <unordered_map>
#include <string>
#include <vector>

struct SubItemShell
{
    std::string shellPrefix;
    std::string shellSuffix;

    SubItemShell() = default;
    SubItemShell(std::string prefix, std::string suffix)
        : shellPrefix(std::move(prefix)), shellSuffix(std::move(suffix)) {}
};

class SubItemProcessor
{
public:

    explicit SubItemProcessor(MDictConfig  dictionaryConfig);

    int processSubItems(
        const pugi::xml_document& xmlDoc,
        std::unordered_map<int, std::vector<std::string>>& keys,
        int pageId,
        MDictExporter& mdictExporter);

private:
    void createSubItemShell(const pugi::xml_node& subItemNode);

    [[nodiscard]] std::string wrapContent(const pugi::xml_node& subItemNode) const;

    MDictConfig dictionaryConfig;
    /*EntryWriter& entryWriter;*/
    bool shellConstructed = false;
    SubItemShell subItemShell;
};

#endif
