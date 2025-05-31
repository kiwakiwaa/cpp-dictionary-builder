#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <filesystem>

struct AssetConfig
{
    std::filesystem::path cssDirectory;
    std::filesystem::path assetDirectory;
    std::filesystem::path iconPath;
    bool prserveDirectoryStructure = true;
    bool overwriteExisting = true;

    void setCssDirectory(const std::filesystem::path& directoryPath)
    {
        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath))
        {
            cssDirectory = directoryPath;
        }
    }

    void setAssetDirectory(const std::filesystem::path& directoryPath)
    {
        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath))
        {
            assetDirectory = directoryPath;
        }
    }

};


class AssetManager
{
public:
    explicit AssetManager(const std::filesystem::path& outputDirectory);

    /**
     * Copy all specified assets to the output directory
     * @param config Asset config specifying what to copy
     */
    void copyAssets(const AssetConfig& config);

    /**
     * Copy entire directory to output directory
     * @param directoryPath Directory to copy
     * @param createSubFolder Whether to create sub folder for asset files
     */
    void copyDirectory(const std::filesystem::path& directoryPath, bool createSubFolder) const;

private:

    /**
     * Get relative path for preserving directory structure
     * @param sourcePath Original source path
     * @param basePath Base path to make relative form
     * @return Relative path
     */
    static std::filesystem::path getRelativePath(const std::filesystem::path& sourcePath, const std::filesystem::path& basePath);


    /**
     * Copy a single file to destination
     * @param source Source file path
     * @param destination Destination file path
     */
    void copyFile(const std::filesystem::path& source, const std::filesystem::path& destination) const;

    std::filesystem::path outputDirectory;
    bool overWriteExisting = true;
};


#endif
