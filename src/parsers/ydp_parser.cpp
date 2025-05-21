#include "yomitan_dictionary_builder/parsers/YDP/ydp_parser.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"

bool YDPParser::processFile(const std::filesystem::path &filePath)
{
    int count = 0;
    const auto entryKeys = indexReader->getKeysForFile(filePath.filename().string());

    pugi::xml_document doc;
    if (!FileUtils::loadXMLFile(doc, filePath))
    {
        std::cerr << "Failed to load xml file " << filePath.string() << std::endl;
    }

    std::string headword = "";



}
