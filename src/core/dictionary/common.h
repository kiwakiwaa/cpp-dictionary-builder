#ifndef DICTIONARY_COMMON_H
#define DICTIONARY_COMMON_H

#include <string_view>
#include <set>

namespace Yomitan
{
    static std::set<std::string_view> allowedElements {
        "br", "ruby", "rt", "rp", "table", "thead", "tbody", "tfoot", "tr",
        "td", "th", "span", "div", "ol", "ul", "li", "img", "a", "details", "summary"
    };

    static std::set<std::string_view> allowedHrefElements {"a"};
}

#endif