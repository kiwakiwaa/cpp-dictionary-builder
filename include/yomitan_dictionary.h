// Header only file containing tools for creating html elements, dictionary entries and yomitan dictionaries

/**
 * html_element.h
*/
#ifndef HTML_ELEMENT_H
#define HTML_ELEMENT_H

#include <glaze/glaze.hpp>
#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <optional>
#include <map>
#include <variant>
#include <memory>
#include <iostream>

class HTMLElement;

const std::set<std::string_view> yomitanAllowedElements {
    "br", "ruby", "rt", "rp", "table", "thead", "tbody", "tfoot", "tr",
    "td", "th", "span", "div", "ol", "ul", "li", "img", "a", "details", "summary"
};
const std::set<std::string_view> yomitanAllowedHrefElements {"a"};

using HTMLElementContent = std::variant<std::string, std::shared_ptr<HTMLElement>>;

// TODO: Change to use unique pointers

class HTMLElement {
public:
    friend struct glz::meta<HTMLElement>;

    explicit HTMLElement(const std::string&  tag) : tag(tag) {}
    HTMLElement(const std::string& tag, const HTMLElementContent& content) : tag(tag), content(std::vector{content}) {}
    HTMLElement(const std::string& tag, const std::vector<HTMLElementContent>& content) : tag(tag), content(content) {}

    void addContent(const std::string& textContent)
    {
        if (content)
            content->emplace_back(textContent);
        else
            content = std::vector<HTMLElementContent> {textContent };
    }

    void addContent(const std::shared_ptr<HTMLElement>& element)
    {
        if (content)
            content->emplace_back(element);
        else
            content = std::vector<HTMLElementContent> {element };
    }

    void setHref(const std::string& value)
    {
        href = value;
    }

    void setData(const std::map<std::string, std::string>& value)
    {
        data = value;
    }

    const std::string& getTag() const
    {
        return tag;
    }

    const std::optional<std::vector<HTMLElementContent>>& getContent() const
    {
        return content;
    }

    std::optional<std::string> getHref() const
    {
        return href;
    }

    std::optional<std::map<std::string, std::string>> getData() const
    {
        return data;
    }

    void print()
    {
        std::string json;
        if (const auto ec = glz::write_json(this, json); ec)
            std::cerr << "Error: " << glz::format_error(ec, json) << std::endl;

        std::cout << "content: " << glz::prettify_json(json) << std::endl;
    }

private:
    std::string tag;
    std::optional<std::vector<HTMLElementContent>> content;
    std::optional<std::string> href;
    std::optional<std::map<std::string, std::string>> data;
};

template <>
struct glz::meta<HTMLElement>
{
    static constexpr auto value = glz::object(
        "tag", &HTMLElement::tag,
        "content", &HTMLElement::content,
        "href", &HTMLElement::href,
        "data", &HTMLElement::data
    );
};

std::shared_ptr<HTMLElement> createHtmlElement(
    const std::string&  tag,
    const std::optional<std::variant<HTMLElementContent, std::vector<HTMLElementContent>>>& content = std::nullopt,
    const std::optional<std::string>& href = std::nullopt,
    const std::optional<std::map<std::string, std::string>>& data = std::nullopt
);

#endif


/**
 * dicentry.h
 */
#ifndef DICENTRY_H
#define DICENTRY_H

#include <glaze/core/meta.hpp>

using StructuredContent = std::map<std::string, std::variant<std::string, std::vector<std::shared_ptr<HTMLElement>>>>;

struct DicEntryFormat
{
    std::string term;
    std::string reading;
    std::string infoTag;
    std::string posTag;
    int searchRank;
    std::vector<StructuredContent> content;
    long sequenceNumber;
    std::string extraField;
};

class DicEntry {

public:
    friend glz::meta<DicEntry>;

    explicit DicEntry(const std::string& term, const std::string& reading)
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

    void addElement(const std::shared_ptr<HTMLElement>& element)
    {
        validateElement(element);
        content.emplace_back(element);
    }

    [[nodiscard]] DicEntryFormat toList() const
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

    void printContent() const
    {
        std::string json;
        DicEntryFormat format = toList();
        if (const auto ec = glz::write_json(format, json); ec)
            std::cerr << "Error: " << glz::format_error(ec, json) << std::endl;

        std::cout << "entry content: " << glz::prettify_json(json) << std::endl;
    }

private:
    static void validateElement(const std::shared_ptr<HTMLElement>& element)
    {
        if (!yomitanAllowedElements.contains(element->getTag()))
            throw std::invalid_argument("Unsupported HTML element: " + element->getTag());

        if (element->getTag().contains("href") && !yomitanAllowedHrefElements.contains(element->getTag()))
            throw std::invalid_argument("The 'href' attribute is not allowed in the '" + element->getTag() + "' element");
    }

    std::string term;
    std::string reading;
    std::string infoTag;
    std::string posTag;
    int searchRank = 0;
    std::vector<std::shared_ptr<HTMLElement>> content;
    long sequenceNumber = 0;
};

template <>
struct glz::meta<DicEntryFormat>
{
    static constexpr auto value = glz::array(
        &DicEntryFormat::term,
        &DicEntryFormat::reading,
        &DicEntryFormat::infoTag,
        &DicEntryFormat::posTag,
        &DicEntryFormat::searchRank,
        &DicEntryFormat::content,
        &DicEntryFormat::sequenceNumber,
        &DicEntryFormat::extraField
    );
};

template <>
struct glz::meta<DicEntry>
{
    static constexpr auto value = glz::array(
        &DicEntry::term,
        &DicEntry::reading,
        &DicEntry::infoTag,
        &DicEntry::posTag,
        &DicEntry::searchRank,
        &DicEntry::content,
        &DicEntry::sequenceNumber,
        ""
    );
};

#endif


/**
 * yomitan_dictionary.h
 */
#ifndef YOMITAN_DICTIONARY_H
#define YOMITAN_DICTIONARY_H

#include "../src/utils/file_utils.h"

struct DictionaryIndex
{
    std::string title;
    std::string author;
    std::string url;
    std::string description;
    std::string attribution;
    int format;
    std::string revision;
};

class YomitanDictionary
{
public:
    friend glz::meta<DicEntry>;

    explicit YomitanDictionary(const std::string_view dictionaryName) : dictionaryName(dictionaryName) {}

    bool addEntry(std::unique_ptr<DicEntry>& entry)
    {
        try
        {
            currentChunk.emplace_back(std::move(entry));
        }
        catch (const std::invalid_argument& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }

        // Check if we should flush the current entry to disk
        if (currentChunk.size() >= CHUNK_SIZE)
        {
            flushChunkToDisk();
            currentChunk.clear();
        }

        return true;
    }

    // TODO: move term banks to output path and export index
    void exportDictionary(std::string_view outputPath) const
    {
        exportIndex(outputPath);
    }

private:
    std::filesystem::path tempDir {"/Users/caoimhe/Downloads/test-dictionary"};
    const size_t CHUNK_SIZE = 10'000;
    bool tempDirIsCleaned = false;

    void flushChunkToDisk()
    {
        try
        {
            if (!std::filesystem::exists(tempDir))
                std::filesystem::create_directories(tempDir);

            if (!tempDirIsCleaned)
            {
                FileUtils::cleanTermBankDirectory(tempDir.string());
                tempDirIsCleaned = true;
            }

            const auto nextTermBankNumber = FileUtils::getNextTermBankNumber(tempDir);
            const std::string_view filename {"term_bank_" + std::to_string(nextTermBankNumber) + ".json"};
            const std::filesystem::path termBankPath {tempDir / filename};

            std::ofstream termBankFile {termBankPath, std::ios::trunc};
            if (!termBankFile.is_open())
                std::cerr << "Could not open term bank file: " << termBankPath << std::endl;

            std::string termBankJson;
            if (const auto ec = glz::write_json(currentChunk, termBankJson); ec)
                std::cerr << "Error: " << glz::format_error(ec, termBankJson) << std::endl;

            termBankFile << glz::prettify_json(termBankJson);
        }
        catch (std::filesystem::filesystem_error& e)
        {
            std::cerr << e.what() << e.path1() << e.code().value() << e.code().message() << std::endl;
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    // TODO: Fix handling for paths with spaces
    void exportIndex(std::string_view outputPath) const
    {
        try
        {
            const std::filesystem::path indexFilePath {(std::filesystem::path(outputPath).parent_path() / "index.json").string()};
            std::ofstream indexFile{indexFilePath, std::ios::trunc};
            if (!indexFile.is_open())
            {
                std::cerr << "Failed to open 'index.json' for writing at: " + indexFilePath.string() << std::endl;
            }

            DictionaryIndex index{
                dictionaryName,
                FileUtils::getUsernameFolder(),
                "",
                "",
                "",
                3,
                "",
            };

            std::string indexJson;
            if (const auto ec = glz::write_json(index, indexJson); ec)
                std::cerr << "Error: " << glz::format_error(ec, indexJson) << std::endl;

            indexFile << glz::prettify_json(indexJson);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << e.what() << e.path1() << e.code().value() << e.code().message() << std::endl;
        }
    }

    std::vector<std::unique_ptr<DicEntry>> currentChunk;
    std::string dictionaryName;
};

template<>
struct glz::meta<DictionaryIndex>
{
    static constexpr auto value = glz::object(
        "title", &DictionaryIndex::title,
        "author", &DictionaryIndex::author,
        "url", &DictionaryIndex::url,
        "description", &DictionaryIndex::description,
        "attribution", &DictionaryIndex::attribution,
        "format", &DictionaryIndex::format,
        "revision", &DictionaryIndex::revision
    );
};

template <>
struct glz::meta<std::vector<DicEntry>>
{
    static constexpr auto value = glz::array(
        &DicEntry::toList
    );
};

#endif

