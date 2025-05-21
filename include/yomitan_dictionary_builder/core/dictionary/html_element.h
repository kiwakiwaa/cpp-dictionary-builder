#ifndef HTML_ELEMENT_H
#define HTML_ELEMENT_H

#include "yomitan_dictionary_builder/core/dictionary/common.h"
#include <glaze/glaze.hpp>
#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <optional>
#include <unordered_map>
#include <variant>
#include <memory>

#include "html_element.h"

class HTMLElement;

using HTMLElementContent = std::variant<std::string, std::shared_ptr<HTMLElement>>;

// TODO: Change to use unique pointers

class HTMLElement {
public:
    friend struct glz::meta<HTMLElement>;

    explicit HTMLElement(const std::string&  tag);
    HTMLElement(const std::string& tag, const HTMLElementContent& content);
    HTMLElement(const std::string& tag, const std::vector<HTMLElementContent>& content);

    void addContent(const std::string& textContent);
    void addContent(const std::shared_ptr<HTMLElement>& element);
    void setHref(const std::string& value);
    void setData(const std::unordered_map<std::string, std::string>& value);

    const std::string& getTag() const;
    const std::optional<std::vector<HTMLElementContent>>& getContent() const;
    std::optional<std::string> getHref() const;
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

std::shared_ptr<HTMLElement> createHtmlElement(
    const std::string&  tag,
    const std::optional<std::variant<HTMLElementContent, std::vector<HTMLElementContent>>>& content = std::nullopt,
    const std::optional<std::string>& href = std::nullopt,
    const std::optional<std::unordered_map<std::string, std::string>>& data = std::nullopt
);

#endif