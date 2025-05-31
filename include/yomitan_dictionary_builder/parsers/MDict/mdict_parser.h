#ifndef MDICT_PARSER_H
#define MDICT_PARSER_H

#include "yomitan_dictionary_builder/core/xml_parser.h"
#include "yomitan_dictionary_builder/index/jukugo_index_reader.h"
#include "../../strategies/link/mdict_link_handling_strategy.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_config.h"
#include "yomitan_dictionary_builder/parsers/MDict/subitem_processor.h"
#include "yomitan_dictionary_builder/parsers/MDict/mdict_exporter.h"
#include "yomitan_dictionary_builder/core/asset_manager.h"

class MdictParser final : public XMLParser
{
public:
    explicit MdictParser(const ParserConfig& config, const MDictConfig& dictionaryConfig);
    ~MdictParser() override;

protected:
    /**
     * Process a single MDict XML file
     * @param filePath Path to the XML file
     * @return Number of keys added to the entry
     */
    int processFile(const std::filesystem::path &filePath) override;

private:
    /**
     * Traverses the whole XML document and fixes link elements so that
     * they work in the converted MDict by adding entry://href_value
     * @param xmlDoc XML document
     * @return Processed XML document
     */
    void processAllLinkElements(const pugi::xml_document& xmlDoc) const;


    /**
     * Get raw XML content as string
     * @param xmlDoc XML document
     * @return Raw XML as string
     */
    static std::string getXMLContent(const pugi::xml_document& xmlDoc);


    /**
     * Initialise the parser at the start of processing
     */
    void initializeProcessing() override;


    /**
     * Finalise processing and export all entries
     */
    void finalizeProcessing() override;

    MDictConfig dictionaryConfig;
    std::unique_ptr<JukugoIndexReader> jukugoIndexReader;

    std::unique_ptr<KeyExtractionStrategy> keyExtractionStrategy;
    std::unique_ptr<MDictLinkHandlingStrategy> linkHandlingStrategy;

    std::unique_ptr<SubItemProcessor> subItemProcessor;
    std::unique_ptr<MDictExporter> exporter;
    std::unique_ptr<AssetManager> assetManager;
};


#endif
