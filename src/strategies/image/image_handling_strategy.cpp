#include <set>
#include <filesystem>
#include <iostream>

#include "yomitan_dictionary_builder/strategies/image/image_handling_strategy.h"

void ImageHandlingStrategy::processAllImageElements(const pugi::xml_document &xmlDoc) const
{
    try
    {
        // Use a set to track processed nodes and avoid duplicates
        std::set<pugi::xml_node> processedNodes;

        // Process all elements with src attributes
        for (const auto imgNodes = xmlDoc.select_nodes("//*[@src]"); auto imgNode : imgNodes)
        {
            pugi::xml_node node = imgNode.node();

            // Skip if already processed
            if (processedNodes.contains(node))
                continue;

            this->processImageElement(node);
            processedNodes.insert(node);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing image elements: " << e.what() << std::endl;
    }
}


std::optional<std::filesystem::path> ImageHandlingStrategy::getImagePath(const pugi::xml_node &xmlNode)
{
    if (!xmlNode) return std::nullopt;

    if (const auto srcAttr = xmlNode.attribute("src"))
    {
        return  std::filesystem::path(srcAttr.value());
    }

    return std::nullopt;
}


