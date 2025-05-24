#ifndef DICENTRY_H
#define DICENTRY_H

#include <string>
#include <glaze/core/meta.hpp>

#include "common.h"
#include "yomitan_dictionary_builder/core/dictionary/html_element.h"

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

	/**
	 * Creates a new dictionary entry with a specified term and reading
	 * @param term The term of the entry
	 * @param reading The reading of the entry
	 */
	explicit DicEntry(const std::string& term, const std::string& reading);

	/**
	 * Appends a new HTML element to the entry contents
	 * @param element Pointer to the HTML element to add
	 */
	void addElement(const std::shared_ptr<HTMLElement>& element);

	/**
	 * Sets the info tag for the entry
	 * @param infoTag The info tag
	 */
	void setInfoTag(const std::string& infoTag);

	/**
	 * Sets the part-of-speech tag for the entry
	 * @param posTag The part-of-speech tag
	 */
	void setPosTag(const std::string& posTag);

	/**
	 * Sets the search rank for the entry (negative values are rarer)
	 * @param searchRank The search rank
	 */
	void setSearchRank(int searchRank);

	/**
	 * Sets the sequence number for the entry
	 * @param sequenceNumber The sequence number
	 */
	void setSequenceNumber(long sequenceNumber);

	/**
	 * Gets the info tag for the entry
	 * @return The info tag
	 */
	std::string getInfoTag() const;

	/**
	 * Gets the part-of-speech tag for the entry
	 * @return The part-of-speech tag
	 */
	std::string getPosTag() const;

	/**
	 * Gets the search rank of the entry (negative values are rarer)
	 * @return The search rank
	 */
	int getSearchRank() const;

	/**
	 * Gets the sequence number of the entry
	 * @return The sequence number
	 */
	long getSequenceNumber() const;

	/**
	 * Creates a yomitan structured content entry struct for the entry
	 * @return Struct containing all the entry fields
	 */
	[[nodiscard]] DicEntryFormat toList() const;

	/**
	 * Prints the full content of the entry in json format
	 */
	void printContent() const;
	
private:
	static void validateElement(const std::shared_ptr<HTMLElement>& element) ;

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