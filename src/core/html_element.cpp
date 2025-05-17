#include "html_element.h"
#include <iostream>

HTMLElement::HTMLElement(const std::string&  tag) : tag(tag) {}

HTMLElement::HTMLElement(const std::string& tag, const HTMLElementContent& content) : tag(tag), content(std::vector{content}) {}

HTMLElement::HTMLElement(const std::string& tag, const std::vector<HTMLElementContent>& content) : tag(tag), content(content) {}

void HTMLElement::addContent(const std::string& textContent)
{
    if (content)
        content->emplace_back(textContent);
    else
        content = std::vector<HTMLElementContent> {textContent };
}

void HTMLElement::addContent(const std::shared_ptr<HTMLElement>& element)
{
    if (content)
        content->emplace_back(element);
    else
        content = std::vector<HTMLElementContent> {element };
}

void HTMLElement::setHref(const std::string& value)
{
    href = value;
}

void HTMLElement::setData(const std::map<std::string, std::string>& value)
{
    data = value;
}

const std::string& HTMLElement::getTag() const
{
    return tag;
}

const std::optional<std::vector<HTMLElementContent>>& HTMLElement::getContent() const
{
    return content;
}

std::optional<std::string> HTMLElement::getHref() const
{
    return href;
}

std::optional<std::map<std::string, std::string>> HTMLElement::getData() const
{
    return data;
}

void HTMLElement::print()
{
    std::string json;
    if (const auto ec = glz::write_json(*this, json); ec)
        std::cerr << "Error: " << glz::format_error(ec, json) << std::endl;

    std::cout << "content: " << glz::prettify_json(json) << std::endl;
}


/*
 * Create Html Element
 **/
std::shared_ptr<HTMLElement> createHtmlElement(
    const std::string&  tag,
    const std::optional<std::variant<HTMLElementContent, std::vector<HTMLElementContent>>>& content,
    const std::optional<std::string>& href,
    const std::optional<std::map<std::string, std::string>>& data
)
{
    if (tag.empty())
        return nullptr;

    std::shared_ptr<HTMLElement> element;
    if (content.has_value())
    {
        if (std::holds_alternative<HTMLElementContent>(content.value()))
            element = std::make_shared<HTMLElement>(tag, std::get<HTMLElementContent>(content.value()));
        else
            element = std::make_shared<HTMLElement>(tag, std::get<std::vector<HTMLElementContent>>(content.value()));
    }
    else
        element = std::make_shared<HTMLElement>(tag);


    if (href)
        element->setHref(href.value());

    if (data)
        element->setData(data.value());

    return element;
}
