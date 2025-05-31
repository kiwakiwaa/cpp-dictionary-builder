#ifndef JUKUGO_INDEX_READER_H
#define JUKUGO_INDEX_READER_H

#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>

class JukugoIndexReader
{
public:

    explicit JukugoIndexReader(std::string_view indexPath);

    bool loadIndex();

    [[nodiscard]] const std::unordered_map<int, std::vector<std::string>>& getGroupedEntriesForPage(int page);

private:

    std::unordered_map<int, std::unordered_map<int, std::vector<std::string>>> groupedEntries;

    std::string indexPath;
};


#endif
