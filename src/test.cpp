#include "core/dictionary/html_element.h"
#include "core/dictionary/dicentry.h"
#include "core/dictionary/yomitan_dictionary.h"
#include "core/xml_parser.h"
#include "core/parser.h"

#include <iostream>

void testCreateHtmlElement()
{
    std::cout << "\n----- Test Create Html Element -----" << std::endl;
    const auto element = createHtmlElement("span", "test content", "maps.apple.com");

    const auto nestedElement = createHtmlElement("span", element);
    nestedElement->addContent(createHtmlElement("span", "test content"));
    nestedElement->print();
}

void testInstantiateHtmlElement()
{
    std::cout << "\n----- Test Instantiate Html Element -----" << std::endl;
    const auto tagOnlyElement = std::make_shared<HTMLElement>("span");
    const auto textOnlyElement = std::make_shared<HTMLElement>("span", "content string");

    tagOnlyElement->print();
    textOnlyElement->print();

    tagOnlyElement->addContent(textOnlyElement);
    tagOnlyElement->print();
}

void testGetMethods()
{
    std::cout << "\n----- Test Get Methods -----" << std::endl;
    const auto element = std::make_shared<HTMLElement>("span", "content string");
    element->setHref("maps.apple.com");
    element->setData({{"見出しG", ""}});
    element->print();
}

void testDicEntry()
{
    std::cout << "\n----- Test DicEntry -----" << std::endl;
    const auto element = std::make_shared<HTMLElement>("span", "content string");
    element->setHref("maps.apple.com");
    const auto nestedElement = std::make_shared<HTMLElement>("span", element);
    nestedElement->addContent(createHtmlElement("span", "additional content"));

    DicEntry entry {"漢字", "かんじ"};
    entry.addElement(nestedElement);
    entry.printContent();
}

void testCreateDictionary()
{
    YomitanDictionary dictionary {"test-dictionary"};
    auto entry = std::make_unique<DicEntry>("豊葦原瑞穂国", "とよあしはらのみずほのくに");
    entry->addElement(createHtmlElement("div", "日本の別称"));

    auto entry2 = std::make_unique<DicEntry>("麒麟", "きりん");
    entry2->addElement(createHtmlElement("div", "🦒科の哺乳動物"));

    auto entry3 = std::make_unique<DicEntry>("白河夜船", "しらかわよふね");
    entry3->addElement(createHtmlElement("div", "知ったかぶり"));

    dictionary.addEntry(entry);
    dictionary.addEntry(entry2);
    dictionary.addEntry(entry3);
    dictionary.exportDictionary("/Users/caoimhe/Downloads/test-dictionary");
}

void testCreateDictionaryWithConfig()
{
    const YomitanDictionaryConfig dictionaryConfig {
        "test-dictionary",
        "bint",
        "",
        "",
        "only me",
        "",
        3,
        3,
        true,
    };

    YomitanDictionary dictionary {dictionaryConfig};

    auto entry = std::make_unique<DicEntry>("豊葦原瑞穂国", "とよあしはらのみずほのくに");
    entry->addElement(createHtmlElement("div", "日本の別称"));

    auto entry2 = std::make_unique<DicEntry>("麒麟", "きりん");
    entry2->addElement(createHtmlElement("div", "🦒科の哺乳動物"));

    auto entry3 = std::make_unique<DicEntry>("白河夜船", "しらかわよふね");
    entry3->addElement(createHtmlElement("div", "知ったかぶり"));

    dictionary.addEntry(entry);
    dictionary.addEntry(entry2);
    dictionary.addEntry(entry3);
    dictionary.exportDictionary("/Users/caoimhe/Downloads/test-dictionary");
}

void testXMLParse()
{
    std::cout << "\n ------ Test XML Parser ------" << std::endl;

    const std::wstring path {L"/Users/caoimhe/Documents/日本語/Dictionary Conversion/yomitan-dictionary-builder/src/test.xml"};
    pugi::xml_document doc;

    if (const pugi::xml_parse_result result = doc.load_file(path.c_str()); !result)
    {
        std::cerr << "Failed to read xml: " << std::endl;
        throw std::runtime_error {"Error reading file"};
        //dieeeee
    }

    const pugi::xml_node root = doc.document_element();
    if (!root)
    {
        std::cerr << "XML has no root" << std::endl;
    }

    XMLParser parser{};
    if (const auto xmlTree = parser.convertElementToYomitan(root); xmlTree)
    {
        std::cout << "successfully parsed xml" << std::endl;
        xmlTree->print();
    }
    else
    {
        std::cerr << "Failed to parse xml" << std::endl;
    }
}

void testParserClass()
{
    std::cout << "\n ------ Test Parser Class ------" << std::endl;
    const std::wstring path {L"/Users/caoimhe/Documents/日本語/Dictionary Conversion/yomitan-dictionary-builder/src/test.xml"};
    pugi::xml_document doc;

    if (const pugi::xml_parse_result result = doc.load_file(path.c_str()); !result)
    {
        std::cerr << "Failed to read xml: " << std::endl;
        throw std::runtime_error {"Error reading file"};
        //dieeeee
    }

    const Parser parser {"大漢和辞典"};
    auto result = parser.parseEntry(
        "鬼哭啾々",
        "きこくしゅうしゅう",
        doc
    );

    if (!result)
    {
        std::cerr << "Failed to parse entry" << std::endl;
    }

    result = parser.exportDictionary("/Users/caoimhe/Downloads/test-dictionary");
    if (!result)
    {
        std::cerr << "Failed to export dictionary" << std::endl;
    }
}

void runAllTests()
{
    testCreateHtmlElement();
    testInstantiateHtmlElement();
    testGetMethods();
    testDicEntry();
    testCreateDictionary();
    testCreateDictionaryWithConfig();
    testXMLParse();
    testParserClass();
}

int main()
{
    runAllTests();
    return 0;
}