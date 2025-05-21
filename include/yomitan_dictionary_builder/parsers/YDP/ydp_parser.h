#ifndef YDP_PARSER_H
#define YDP_PARSER_H

#include "yomitan_dictionary_builder/core/parser.h"

class YDPParser final : public Parser
{
public:

    bool processFile(const std::filesystem::path &filePath) override;

private:

};

#endif