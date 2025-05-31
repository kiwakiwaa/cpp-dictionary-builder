#include "yomitan_dictionary_builder/core/xml_parser.h"
#include <sstream>


XMLParser::XMLParser(const ParserConfig& config) : BaseParser(config)
{
    if (config.tagMappingPath.has_value())
    {
        loadTagMapping(config.tagMappingPath.value());
    }
    this->indexReader = config.indexPath.has_value() ? std::make_unique<IndexReader>(config.indexPath.value().string()) : nullptr;
}


// NOLINTNEXTLINE(misc-no-recursion)
std::string XMLParser::getTargetTag(
    const std::string& tagName,
    const std::optional<std::vector<std::string>>& classList,
    const std::optional<pugi::xml_node>& parent,
    const std::optional<int> recursionDepth) const
{
    if (Yomitan::allowedElements.contains(tagName))
    {
        return tagName;
    }

    thread_local std::string selectorBuffer;

    // Look for nested rules
    if (hasParentSelectors && parent.has_value())
    {
        const auto& parentNode = parent.value();
        const char* parentName = parentNode.name();

        // Try parent.class + tag
        if (const auto parentClassList = getClassList(parent.value()); parentClassList.has_value())
        {
            for (const auto& parentClass : parentClassList.value())
            {
                selectorBuffer.clear();
                selectorBuffer.reserve(strlen(parentName) + parentClass.size() + tagName.size() + 2);
                selectorBuffer.append(parentName);
                selectorBuffer.append(".");
                selectorBuffer.append(parentClass);
                selectorBuffer.append(" ");
                selectorBuffer.append(tagName);

                if (const auto it = this->tagMapping.find(selectorBuffer); it != this->tagMapping.end())
                {
                    return it->second;
                }
            }
        }


        selectorBuffer.clear();

        selectorBuffer.reserve(strlen(parent->name()) + tagName.size() + 1);
        selectorBuffer.append(parentName);
        selectorBuffer.append(" ");
        selectorBuffer.append(tagName);

        if (const auto it = this->tagMapping.find(selectorBuffer); it != this->tagMapping.end())
        {
            return it->second;
        }

        // Recurse up the ancestor chain if no match
        if (recursionDepth.has_value() && recursionDepth.value() < 5 && parent->parent() != nullptr)
        {
            auto targetTag = getTargetTag(
                tagName,
                classList,
                parent->parent(),
                recursionDepth.value() + 1
                );
            return targetTag;
        }
    }

    // Try tag.class (no parent involvement)
    if (classList.has_value())
    {
        for (const auto& className : classList.value())
        {
            selectorBuffer.clear();
            selectorBuffer.reserve(tagName.size() + className.size() + 1);
            selectorBuffer.append(tagName);
            selectorBuffer.append(".");
            selectorBuffer.append(className);

            if (const auto it = this->tagMapping.find(selectorBuffer); it != this->tagMapping.end())
            {
                return it->second;
            }
        }
    }

    // Fall back to regular tag mapping or default
    if (const auto it = this->tagMapping.find(tagName); it != this->tagMapping.end())
    {
        return it->second;
    }

    return "span";
}


std::optional<std::vector<std::string>> XMLParser::getClassList(const pugi::xml_node &node)
{
    if (const pugi::xml_attribute classAttribute = node.attribute("class"))
    {
        if (const std::string classValue = classAttribute.value() ? classAttribute.value() : ""; !classValue.empty())
        {
            std::vector<std::string> classList;
            std::istringstream iss(classValue);
            std::string className;
            while (iss >> className)
            {
                std::ranges::replace(className, '-', '_');
            }
            classList.emplace_back(className);
            return classList;
        }
    }
    return std::nullopt;
}


std::unordered_map<std::string, std::string> XMLParser::getAttributeData(const pugi::xml_node &node)
{
    std::unordered_map<std::string, std::string> dataMap;

    thread_local std::string processedName;

    for (pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute())
    {
        const char* attrNamePtr = attr.name();
        const char* attrValuePtr = attr.value();

        if (!attrNamePtr || !attrValuePtr)
            continue;

        std::string_view attrName(attrNamePtr);
        std::string_view attrValue(attrValuePtr);

        if (attrName.empty() || attrValue == "class")
            continue;

        if (attrValue.find(".css") != std::string_view::npos || attrValue == "viewport")
            continue;

        if (ignoredAttributes.contains(attrName))
            continue;

        processedName.assign(attrName);
        std::ranges::replace(processedName, '-', '_');
        dataMap.emplace(std::move(processedName), std::string{attrValue});
    }
    return dataMap;
}

// NOLINTNEXTLINE(misc-no-recursion)
std::shared_ptr<HTMLElement> XMLParser::convertElementToYomitan(const pugi::xml_node& node, bool ignoreExpressions) const
{
    if (node == nullptr)
        return {nullptr};

    auto dataMap = getAttributeData(node);
    const auto classList = getClassList(node);
    const auto tagName = hasParentSelectors ? getTargetTag(node.name(), classList, node.parent()) : getTargetTag(node.name());

    std::shared_ptr<HTMLElement> element;

    if (Yomitan::allowedElements.contains(tagName))
    {
        element = std::make_shared<HTMLElement>(tagName);
    }
    else
    {
        element = std::make_shared<HTMLElement>("span");
        element->setData({{tagName, ""}});
    }

    if (classList.has_value())
    {
        for (const auto& className : classList.value())
        {
            dataMap[className] = "";
        }
    }

    if (!dataMap.empty())
    {
        element->setData(dataMap);
    }

    for (pugi::xml_node child = node.first_child(); child != nullptr; child = child.next_sibling())
    {
        if (child.type() == pugi::node_pcdata || child.type() == pugi::node_cdata)
        {
            if (const std::string textContent = child.value(); !textContent.empty())
            {
                element->addContent(textContent);
            }
        }
        else if (child.type() == pugi::node_element)
        {
            if (auto childElement = convertElementToYomitan(child))
            {
                element->addContent(childElement);
            }
        }
    }

    return element;
}


// NOLINTNEXTLINE(misc-no-recursion)
std::string XMLParser::getElementText(const pugi::xml_node& node, const std::optional<std::set<std::string>>& ignoredElements)
{
    std::string collectedText;
    for (pugi::xml_node child = node.first_child(); child != nullptr; child = child.next_sibling())
    {
        if (ignoredElements.has_value())
            if (ignoredElements->contains(child.name()))
                continue;

        if (const auto childType = child.type(); childType == pugi::node_pcdata || childType == pugi::node_cdata)
        {
            if (const std::string textContent = child.value(); !textContent.empty())
            {
                collectedText += textContent;
            }
        }
        else if (childType == pugi::node_element)
        {
            collectedText += getElementText(child, ignoredElements);
        }
    }
    return collectedText;
}


void XMLParser::loadTagMapping(const std::filesystem::path &filePath)
{
    if (!this->config.tagMappingPath.has_value())
        return;

    const auto json = FileUtils::readFile(this->config.tagMappingPath.value());
    if (!json.has_value())
        return;

    try
    {
        if (const auto ec = glz::read_json(this->tagMapping, json.value()))
            std::cerr << "Error reading tag map: " << glz::format_error(ec, json.value()) << std::endl;

        // Check if there are any parent selectors in the tag mapping
        // to avoid unecessary recursion when getting tags later
        auto predicate = [](const std::string& s) {
            return s.contains('.') || s.contains(' ');
        };

        if (std::ranges::any_of(tagMapping | std::views::keys, predicate)) {
            hasParentSelectors = true;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error reading tag map: " << e.what() << std::endl;
    }
}