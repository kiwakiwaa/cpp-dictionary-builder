#include "core/html_element.h"
#include "core/dicentry.h"
#include "core/yomitan_dictionary.h"

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

    dictionary.addEntry(entry);
    dictionary.exportDictionary("/Users/caoimhe/Downloads/test-dictionary");
}

void runAllTests()
{
    testCreateHtmlElement();
    testInstantiateHtmlElement();
    testGetMethods();
    testDicEntry();
    testCreateDictionary();
}

int main()
{
    runAllTests();
    return 0;
}