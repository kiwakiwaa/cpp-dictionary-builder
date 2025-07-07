#ifndef IMAGE_HANDLING_STRATEGY_H
#define IMAGE_HANDLING_STRATEGY_H

#include "pugixml.h"

#include <optional>

class ImageHandlingStrategy
{
public:
    ImageHandlingStrategy() = default;
    virtual ~ImageHandlingStrategy() = default;

    /**
     * Traverses the whole XML document and processes any image elments' source paths
     *
     * @param xmlDoc XML document
     */
    void processAllImageElements(const pugi::xml_document& xmlDoc) const;


protected:

    /**
     * Processes a single image element
     *
     * @param xmlNode XML document
     */
    virtual void processImageElement(const pugi::xml_node& xmlNode) const = 0;


    /**
     * Gets the image path from the src attribute of an XML element, e.g 'graphics/filename.png'
     *
     * @param xmlNode the XML node
     * @return Image file path
     */
    static std::optional<std::filesystem::path> getImagePath(const pugi::xml_node& xmlNode);

};

// TODO: add default handler

#endif
