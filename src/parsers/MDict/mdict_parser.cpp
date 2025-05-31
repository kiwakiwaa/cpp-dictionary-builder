#include "yomitan_dictionary_builder/parsers/MDict/mdict_parser.h"
#include "yomitan_dictionary_builder/utils/jptools/kanji_utils.h"

#include <complex>
#include <vector>

#include "yomitan_dictionary_builder/strategies/key/key_extraction_strategy.h"


MdictParser::MdictParser(const ParserConfig &config, const MDictConfig& dictionaryConfig) : XMLParser(config), dictionaryConfig(dictionaryConfig)
{
    this->jukugoIndexReader = std::make_unique<JukugoIndexReader>(
        (config.indexPath.value().parent_path() / "jyukugo_prefix.tsv").string());

    // Create strategies
    keyExtractionStrategy = config.createKeyExtractionStrategy();
    linkHandlingStrategy = config.createMDictLinkStrategy(dictionaryConfig);

    subItemProcessor = std::make_unique<SubItemProcessor>(dictionaryConfig);
}


MdictParser::~MdictParser() = default;


void MdictParser::initializeProcessing()
{
    if (!config.outputPath.has_value())
    {
        throw std::runtime_error("Output path is required for MDict parser");
    }

    exporter = std::make_unique<MDictExporter>(dictionaryConfig, config);

    if (config.hasAssets())
    {
        assetManager = std::make_unique<AssetManager>(config.outputPath.value());
    }
}


void MdictParser::finalizeProcessing()
{
    if (config.hasAssets())
    {
        AssetConfig assetConfig;

        if (config.cssDirectory.has_value())
            assetConfig.cssDirectory = config.cssDirectory.value();

        if (config.iconPath.has_value())
            assetConfig.iconPath = config.iconPath.value();

        assetManager->copyAssets(assetConfig);
    }

    if (exporter)
    {
        exporter->finalize();

        if (config.showProgress)
        {
            const auto [totalEntries, totalKeys] = exporter->exportStats();
            std::cout << "Processing complete" << '\n';
            std::cout << "  Total entries: " << totalEntries << '\n';
            std::cout << "  Total keys: " << totalKeys << std::endl;
        }
    }
}


int MdictParser::processFile(const std::filesystem::path &filePath)
{
    pugi::xml_document doc;
    if (!FileUtils::loadXMLFile(doc, filePath))
    {
        std::cerr << "Failed to load xml file " << filePath.string() << std::endl;
        return 0;
    }

    const int pageID = MDictLinkHandlingStrategy::getPageId(filePath.filename().string());

    // fix all link elements e.g ensure "entry://...", "sound://..."
    processAllLinkElements(doc);

    auto headEntryKeys = indexReader->getKeysForFile(filePath.stem().string());

    // get any dictionary specific keys that are missing from the index
    if (headEntryKeys.empty())
        headEntryKeys = keyExtractionStrategy->extractKeys(doc, filePath);

    auto jukugoKeys = jukugoIndexReader->getGroupedEntriesForPage(pageID);

    const int subItemsProcessed = subItemProcessor->processSubItems(doc, jukugoKeys, pageID, *exporter);

    // Remove the subitem section
    for (auto subItemGNode : doc.select_nodes("//SubItemG"))
        subItemGNode.node().parent().remove_child(subItemGNode.node());


    const std::string xmlContent = getXMLContent(doc);
    if (xmlContent.empty())
    {
        std::cerr << "No content extracted from file: " << filePath << std::endl;
        return 0;
    }

    const MDictEntry entry(pageID, headEntryKeys, xmlContent);
    exporter->addEntry(entry);

    return static_cast<int>(headEntryKeys.size()) + subItemsProcessed;
}


std::string MdictParser::getXMLContent(const pugi::xml_document& xmlDoc)
{
    try
    {
        std::ostringstream oss;
        xmlDoc.save(oss, "", pugi::format_raw, pugi::encoding_utf8);
        return oss.str();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error getting XML content: " << e.what() << std::endl;
        return "";
    }
}


void MdictParser::processAllLinkElements(const pugi::xml_document& xmlDoc) const
{
    try
    {
        // Use a set to track processed nodes and avoid duplicates
        std::set<pugi::xml_node> processedNodes;

        // Process all elements with href attributes

        for (const auto linkNodes = xmlDoc.select_nodes("//*[@href]"); auto linkNode : linkNodes)
        {
            pugi::xml_node node = linkNode.node();

            // Skip if already processed
            if (processedNodes.contains(node))
                continue;

            if (pugi::xml_attribute hrefAttr = node.attribute("href"))
            {
                std::string href = hrefAttr.value();
                std::string tagName = node.name();

                // Skip external URLs and entry:// URLs
                if (href.find("http://") == 0 || href.find("https://") == 0 ||
                    href.find("entry://") == 0)
                {
                    processedNodes.insert(node);
                    continue;
                }

                const std::string newHref = linkHandlingStrategy->getNewHref(href);
                if (newHref.empty())
                {
                    processedNodes.insert(node);
                    continue;
                }

                if (tagName == "a")
                {
                    // Direct href replacement for <a> tags
                    hrefAttr.set_value(newHref.c_str());
                }
                else if (!newHref.empty())
                {
                    // Wrap non-<a> elements with clickable link
                    pugi::xml_node parent = node.parent();
                    if (!parent)
                    {
                        processedNodes.insert(node);
                        continue;
                    }

                    // Create wrapper <a> element
                    pugi::xml_node wrapperLink = parent.insert_child_before("a", node);
                    wrapperLink.append_attribute("class") = "element-link-wrapper";
                    wrapperLink.append_attribute("href") = newHref.c_str();

                    if (pugi::xml_node movedNode = wrapperLink.append_move(node))
                    {
                        movedNode.remove_attribute("href");
                        processedNodes.insert(movedNode);
                    }
                }

                processedNodes.insert(node);
            }
        }
    }
    catch (const std::exception& e)
    {
        // Log error but continue processing
        std::cerr << "Error processing link elements: " << e.what() << std::endl;
    }
}