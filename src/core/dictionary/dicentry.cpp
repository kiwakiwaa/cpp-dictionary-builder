#include "dicentry.h"

#include <iostream>

DicEntry::DicEntry(const std::string& term, const std::string& reading)
{
    if (term.empty())
    {
        this->term = reading;
        this->reading = "";
    }
    else
    {
        this->term = term;
        this->reading = reading;
    }
}

void DicEntry::addElement(const std::shared_ptr<HTMLElement> &element)
{
    validateElement(element);
    content.emplace_back(element);
}

void DicEntry::validateElement(const std::shared_ptr<HTMLElement> &element)
{
    if (!Yomitan::allowedElements.contains(element->getTag()))
        throw std::invalid_argument("Unsupported HTML element: " + element->getTag());

    if (element->getTag().contains("href") && !Yomitan::allowedHrefElements.contains(element->getTag()))
        throw std::invalid_argument("The 'href' attribute is not allowed in the '" + element->getTag() + "' element");
}

void DicEntry::setInfoTag(const std::string &infoTag)
{
    this->infoTag = infoTag;
}

void DicEntry::setPosTag(const std::string &posTag)
{
    this->posTag = posTag;
}

void DicEntry::setSearchRank(const int searchRank)
{
    this->searchRank = searchRank;
}

void DicEntry::setSequenceNumber(const long sequenceNumber)
{
    this->sequenceNumber = sequenceNumber;
}

std::string DicEntry::getInfoTag() const
{
    return infoTag;
}

std::string DicEntry::getPosTag() const
{
    return posTag;
}

int DicEntry::getSearchRank() const
{
    return searchRank;
}

long DicEntry::getSequenceNumber() const
{
    return sequenceNumber;
}

DicEntryFormat DicEntry::toList() const
{
    DicEntryFormat result;

    result.term = term;
    result.reading = reading;
    result.infoTag = infoTag;
    result.posTag = posTag;
    result.searchRank = searchRank;

    StructuredContent contentMap;
    contentMap["type"] = "structured-content";
    contentMap["content"] = content;

    result.content = std::vector{contentMap};
    result.sequenceNumber = sequenceNumber;
    result.extraField = std::string("");
    return result;
}

void DicEntry::printContent() const
{
    std::string json;
    DicEntryFormat format = toList();
    if (const auto ec = glz::write_json(format, json); ec)
        std::cerr << "Error: " << glz::format_error(ec, json) << std::endl;

    std::cout << "entry content: " << glz::prettify_json(json) << std::endl;
}

