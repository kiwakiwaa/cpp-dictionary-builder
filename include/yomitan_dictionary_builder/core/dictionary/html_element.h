#ifndef HTML_ELEMENT_H
#define HTML_ELEMENT_H

#include <glaze/glaze.hpp>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <variant>
#include <memory>

#include "html_element.h"

class HTMLElement;

using HTMLElementContent = std::variant<std::string, std::shared_ptr<HTMLElement>>;

class HTMLElement {
public:
    friend struct glz::meta<HTMLElement>;

    /**
     * Creates a new HTML element with a specified tag
     * @param tag The tag name
     */
    explicit HTMLElement(const std::string&  tag);

    /**
     * Creates a new HTML element with a specified tag name and content
     * @param tag The tag name
     * @param content HTML content, either a string or a pointer to another element
     */
    HTMLElement(const std::string& tag, const HTMLElementContent& content);

    /**
     * Creates a new HTML element with a specified tag name and vector of HTML content
     * @param tag The tag name
     * @param content A vector of HTML content
     */
    HTMLElement(const std::string& tag, const std::vector<HTMLElementContent>& content);

    /**
     * Adds simple text content to the contents of the element
     * @param textContent The text content to add
     */
    void addContent(const std::string& textContent);

    /**
     * Adds an element to the contents of the current element
     * @param element Pointer to the element to add
     */
    void addContent(const std::shared_ptr<HTMLElement>& element);

    /**
     * Sets the HTML tag name for the element
     * @param value The tag name
     */
    void setTag(const std::string& value);

    /**
     * Sets the href link for the element
     * @param value  The href value
     */
    void setHref(const std::string& value);

    /**
     * Sets the data attributes for the element with a specified data map
     * @param value The data map
     */
    void setData(const std::unordered_map<std::string, std::string>& value);


    /**
     * Gets the HTML tag name for the element
     * @return The tag name
     */
    const std::string& getTag() const;

    /**
     * Gets the content of the element
     * @return The content or nullopt
     */
    const std::optional<std::vector<HTMLElementContent>>& getContent() const;

    /**
     * Gets the href link for the element
     * @return The href value or nullopt
     */
    std::optional<std::string> getHref() const;

    /**
     * Gets the data map for the element
     * @return The data map or nullopt
     */
    std::optional<std::unordered_map<std::string, std::string>> getData() const;

    void print();

private:
    std::string tag;
    std::optional<std::vector<HTMLElementContent>> content;
    std::optional<std::string> href;
    std::optional<std::unordered_map<std::string, std::string>> data;
};

template <>
struct glz::meta<HTMLElement>
{
    static constexpr auto value = glz::object(
        "tag", &HTMLElement::tag,
        "content", &HTMLElement::content,
        "href", &HTMLElement::href,
        "data", &HTMLElement::data
    );
};

#endif