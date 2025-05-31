#ifndef YDP_YOMITAN_PARSER_H
#define YDP_YOMITAN_PARSER_H

#include "yomitan_dictionary_builder/core/yomitan_parser.h"

namespace YDP
{
    class YomitanParser final : public ::YomitanParser
    {
    public:

        using ::YomitanParser::YomitanParser;

        int processFile(const std::filesystem::path &filePath) override;

    private:
        static std::string extractHeadword(const pugi::xml_node &node);

    };
}

#endif