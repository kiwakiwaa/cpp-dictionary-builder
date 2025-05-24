#ifndef YDP_PARSER_H
#define YDP_PARSER_H

#include "yomitan_dictionary_builder/core/parser.h"

namespace YDP
{
    class Parser final : public ::Parser
    {
    public:

        using ::Parser::Parser;

        int processFile(const std::filesystem::path &filePath) override;

    private:
        static std::string extractHeadword(const pugi::xml_node &node);

    };
}

#endif