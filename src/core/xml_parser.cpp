#include "yomitan_dictionary_builder/core/xml_parser.h"
#include <sstream>

std::unordered_map<std::string, std::string> XMLParser::getAttributeData(const pugi::xml_node &node)
{
    std::unordered_map<std::string, std::string> dataMap;

    for (pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute())
    {
        std::string attrName = attr.name() ? attr.name() : "";
        const std::string attrValue = attr.value() ? attr.value() : "";

        if (ignoredAttributes.contains(attrName) || attrName == "class")
            continue;

        if (attrValue.contains(".css") || attrValue == "viewport")
            continue;

        if (!attrName.empty())
        {
            // need to replace dash w underscore bc yomitan is -_-
            std::string processedName = attrName;
            for (char& c : processedName)
            {
                if (c == '-') c = '_';
            }
            dataMap[processedName] = attrValue;
        }
    }
    return dataMap;
}

std::vector<std::string> XMLParser::getClassList(const pugi::xml_node &node)
{
    std::vector<std::string> classList;

    if (pugi::xml_attribute classAttribute = node.attribute("class"))
    {
        if (std::string classValue = classAttribute.value() ? classAttribute.value() : ""; !classValue.empty())
        {
            std::istringstream iss(classValue);
            std::string className;
            while (iss >> className)
            {
                std::string processedClassName = className;
                for (char& c : processedClassName)
                {
                    if (c == '-') c = '_';
                }
                classList.emplace_back(processedClassName);
            }
        }
    }
    return classList;
}


std::shared_ptr<HTMLElement> XMLParser::convertElementToYomitan(const pugi::xml_node& node, bool ignoreExpressions)
{
    if (node == nullptr)
        return {nullptr};

    std::string tagName = node.name() ? node.name() : "";
    std::shared_ptr<HTMLElement> element;

    if (tagName.empty())
    {
        if (std::string text = node.value(); !text.empty())
        {
            return std::make_shared<HTMLElement>("span", text);
        }
        return nullptr;
    }

    if (Yomitan::allowedElements.contains(tagName))
    {
        element = std::make_shared<HTMLElement>(tagName);
    }
    else
    {
        element = std::make_shared<HTMLElement>("span");
        // Store original tag as attribute
        element->setData({{tagName, ""}});
    }

    auto dataMap = getAttributeData(node);
    auto classList = getClassList(node);
    for (const auto& className : classList)
    {
        dataMap[className] = "";
    }

    if (!dataMap.empty())
    {
        element->setData(dataMap);
    }

    for (pugi::xml_node child = node.first_child(); child != nullptr; child = child.next_sibling())
    {
        if (auto childElement = convertElementToYomitan(child))
        {
            element->addContent(childElement);
        }
    }

    return element;
}
