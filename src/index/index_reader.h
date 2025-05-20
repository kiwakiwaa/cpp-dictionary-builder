#ifndef INDEX_READER_H
#define INDEX_READER_H

#include <string_view>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

class IndexReader
{
public:
    /**
     * Initialises index reader with a specified path
     * @param indexPath Path to dictionary index
     */
    explicit IndexReader(std::string_view indexPath);

    /**
     * Loads the index file and creates 'filename' -> 'keys' mapping
     * @return True if successful
     */
    bool loadIndex();

    /**
     * Gets the dictionary keys for an entry
     * @param filename Filename matching the page number for a dictionary entry
     * @return Vector with the entry keys
     */
    [[nodiscard]] const std::vector<std::string>& getKeysForFile(std::string_view filename);

private:
    std::string indexPath;

    // page number -> keys
    std::unordered_map<std::string, std::vector<std::string>> fileToKeys;

    std::vector<std::string> emptyVector;
};



#endif