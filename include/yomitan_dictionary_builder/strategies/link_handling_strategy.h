#ifndef LINK_HANDLING_STRATEGY_H
#define LINK_HANDLING_STRATEGY_H

#include "yomitan_dictionary_builder/core/dictionary/html_element.h"

class LinkHandlingStrategy
{
public:
    virtual  ~LinkHandlingStrategy() = default;

    virtual std::shared_ptr<HTMLElement> handleLinkElement(
        const pugi::xml_node& node,
        std::string_view targetTag,
        const std::unordered_map<std::string, std::string>& dataAttributes,
        const std::vector<std::string>& classList
    ) = 0;
};


class DefaultLinkHandlingStrategy final : public LinkHandlingStrategy
{
    std::shared_ptr<HTMLElement> handleLinkElement(
        const pugi::xml_node &node,
        std::string_view targetTag,
        const std::unordered_map<std::string, std::string> &dataAttributes,
        const std::vector<std::string> &classList
        ) override;
};

#endif