#ifndef DICENTRY_H
#define DICENTRY_H

#include <string>
#include <glaze/core/meta.hpp>

#include "html_element.h"

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

	explicit DicEntry(const std::string& term, const std::string& reading);

	void addElement(const std::shared_ptr<HTMLElement>& element);
	[[nodiscard]] DicEntryFormat toList() const;
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


#endif