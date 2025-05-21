#ifndef IMAGE_HANDLING_STRATEGY_H
#define IMAGE_HANDLING_STRATEGY_H

#include "../core/dictionary/html_element.h"

class ImageHandlingStrategy
{
public:
    virtual ~ImageHandlingStrategy() = default;

    virtual std::shared_ptr<HTMLElement> handleImageElement(
        const pugi::xml_node& element,
        std::string_view targetTag,
        const std::unordered_map<std::string, std::string>& dataAttributes,
        const std::vector<std::string>& classList
    ) = 0;
};

class DefaultImageHandlingStrategy final : public ImageHandlingStrategy
{
    std::shared_ptr<HTMLElement> handleImageElement(
        const pugi::xml_node &element, std::string_view targetTag,
        const std::unordered_map<std::string, std::string> &dataAttributes,
        const std::vector<std::string> &classList
        ) override;
};

#endif
