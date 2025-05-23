#include "yomitan_dictionary_builder/parsers/YDP/parser.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"
#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

namespace YDP
{
    int Parser::processFile(const std::filesystem::path &filePath)
    {
        int count = 0;
        const auto entryKeys = indexReader->getKeysForFile(filePath.stem().string());

        pugi::xml_document doc;
        if (!FileUtils::loadXMLFile(doc, filePath))
        {
            std::cerr << "Failed to load xml file " << filePath.string() << std::endl;
        }

        const std::string headword = extractHeadword(doc.document_element());

        const auto normalizedKeys = normalizeKeys(entryKeys, headword);
        const auto matchedKeys = KanjiUtils::matchKanaWithKanji(normalizedKeys);

        for (const auto& [kanjiPart, kanaPart] : matchedKeys)
        {
            const auto reading = kanaPart.has_value() ? kanaPart.value() : "";
            const auto term = kanjiPart.has_value() ? kanjiPart.value() : reading;

            if (!term.empty() && !reading.empty() && term != reading)
            {
                //const auto tags = getPartOfSpeechTags(kanjiPart.value());
                count += parseEntry(
                    term, reading, doc,
                    "", "", -1
                );
            }
            else if (!term.empty() && reading.empty() || term == reading)
            {
                count += parseEntry(
                    term, "", doc,
                    "", "", 0
                );
            }
        }

        if (count == 0)
            std::cerr << "No entries parsed for file " << filePath.string() << std::endl;

        return count;
    }


    std::string Parser::extractHeadword(const pugi::xml_node &node)
    {
        auto predicate = [](const pugi::xml_node &n) {
            return std::string(n.name()) == "headword" &&
                    std::string(n.attribute("class").value()) == "見出";
        };

        const auto element = node.find_node(predicate);
        return getElementText(element, {{"lang"}});
    }
}
