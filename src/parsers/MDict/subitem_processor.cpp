#include "yomitan_dictionary_builder/parsers/MDict/subitem_processor.h"
#include "../../../include/yomitan_dictionary_builder/strategies/link/mdict_link_handling_strategy.h"
#include "yomitan_dictionary_builder/utils/jptools/kana_convert.h"

#include <iostream>
#include <sstream>
#include <ranges>
#include <utility>

SubItemProcessor::SubItemProcessor(MDictConfig  dictionaryConfig) : dictionaryConfig(std::move(dictionaryConfig))
{
}


int SubItemProcessor::processSubItems(const pugi::xml_document& xmlDoc, std::unordered_map<int, std::vector<std::string>>& keys, const int pageId, MDictExporter& mdictExporter)
{
    int processedCount = 0;

    try
    {
        static const std::string query {"//" + dictionaryConfig.subElement};
        for (auto subItemNode : xmlDoc.select_nodes(query.c_str()))
        {
            pugi::xml_attribute idAttr = subItemNode.node().attribute("id");
            if (!idAttr) continue;

            const std::string subItemId = idAttr.value();
            const std::string itemId = MDictLinkHandlingStrategy::extractItemId(subItemId);
            const int itemIdVal = std::stoi(itemId);

            // Check if we have keys for this item ID
            if (!keys.contains(itemIdVal))
            {
                std::cerr << "No jukugo keys found for item ID: " << itemId << " in page: " << std::to_string(pageId) << std::endl;
                continue;
            }

            if (!shellConstructed)
                createSubItemShell(subItemNode.node());

            const std::string wrappedContent = wrapContent(subItemNode.node());

            // Create combined entry ID: pageID + itemID
            const long entryId = std::stol(std::to_string(80) + std::to_string(pageId) + itemId);

            // Get keys for this item
            const std::vector<std::string>& itemKeys = keys[itemIdVal];

            MDictEntry entry(entryId, itemKeys, wrappedContent);
            mdictExporter.addEntry(entry);

            processedCount++;
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing SubItems: " << e.what() << std::endl;
    }

    return processedCount;
}


void SubItemProcessor::createSubItemShell(const pugi::xml_node& subItemNode)
{
    std::vector<std::string> parentTags;
    std::string shellPrefix;
    std::string shellSuffix;
    std::string headContent;

    // Find the document root and look for the HTML head element
    pugi::xml_node rootNode = subItemNode;
    while (rootNode.parent())
    {
        rootNode = rootNode.parent();
    }

    // Look for html/head structure in the document
    if (pugi::xml_node htmlNode = rootNode.child("html"))
    {
        if (pugi::xml_node headNode = htmlNode.child("head"))
        {
            std::ostringstream oss;
            for (auto child : headNode.children())
            {
                child.print(oss, "", pugi::format_raw);
            }
            headContent = "<head>" + oss.str() + "</head>";
        }
    }

    // Collect parent tags by walking up the hierarchy (excluding the subItem itself)
    pugi::xml_node currentNode = subItemNode.parent();
    while (currentNode && currentNode.type() == pugi::node_element)
    {
        if (const std::string tagName = currentNode.name(); !tagName.empty())
        {
            parentTags.push_back(tagName);
        }
        currentNode = currentNode.parent();
    }

    // Build prefix: start with html tag, then head, then parent tags in reverse order
    for (auto & parentTag : std::ranges::reverse_view(parentTags))
    {
        if (parentTag == "html")
        {
            shellPrefix += "<html>";
            if (!headContent.empty())
            {
                shellPrefix += headContent;
            }
        }
        else
        {
            shellPrefix += "<" + parentTag + ">";
        }
    }

    // Build suffix: closing tags in forward order
    for (const auto& tag : parentTags)
    {
        shellSuffix += "</" + tag + ">";
    }

    shellConstructed = true;
    subItemShell = SubItemShell(shellPrefix, shellSuffix);
}


std::string SubItemProcessor::wrapContent(const pugi::xml_node& subItemNode) const
{
    // Get only the inner content of the SubItem
    std::ostringstream contentOss;
    for (auto child : subItemNode.children())
    {
        child.print(contentOss, "", pugi::format_raw);
    }
    const std::string subItemContent = contentOss.str();

    return subItemShell.shellPrefix + subItemContent + subItemShell.shellSuffix;
}


