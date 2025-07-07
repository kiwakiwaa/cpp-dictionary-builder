#include "yomitan_dictionary_builder/core/asset_manager.h"

#include <iostream>

AssetManager::AssetManager(const std::filesystem::path& outputDirectory) : outputDirectory(outputDirectory)
{
    try
    {
        std::filesystem::create_directories(outputDirectory);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        throw std::runtime_error("Failed to create output directory: " + outputDirectory.string() + " - " + e.what());
    }
}


void AssetManager::copyAssets(const AssetConfig& config)
{
    overWriteExisting = config.overwriteExisting;

    if (!config.assetDirectory.empty())
    {
        copyDirectory(config.assetDirectory, true);
    }

    if (!config.fontDirectory.empty())
    {
        copyDirectory(config.fontDirectory, false);
    }

    if (!config.cssDirectory.empty())
    {
        copyDirectory(config.cssDirectory, false);
    }

    if (!config.iconPath.empty())
    {
        const std::filesystem::path destinationPath = outputDirectory / config.iconPath.filename();
        copyFile(config.iconPath, destinationPath);
    }
}


void AssetManager::copyDirectory(const std::filesystem::path& directoryPath, const bool createSubFolder) const
{
    try
    {
        if (!std::filesystem::exists(directoryPath))
        {
            throw std::runtime_error("Directory does not exist: " + directoryPath.string());
        }

        if (!std::filesystem::is_directory(directoryPath))
        {
            throw std::runtime_error("Path is not a directory: " + directoryPath.string());
        }

        const auto assetsDirectory = createSubFolder ? outputDirectory / "mdd" : outputDirectory;
        std::filesystem::create_directories(assetsDirectory);

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (!entry.is_regular_file()) continue;

            const auto& sourcePath = entry.path();
            std::filesystem::path destinationPath;

            const auto relativePath = getRelativePath(sourcePath, directoryPath);
            destinationPath = assetsDirectory / relativePath;


            std::filesystem::create_directories(destinationPath.parent_path());

            copyFile(sourcePath, destinationPath);
        }
    }
    catch (std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error copying directory: " << directoryPath.string() << " - " << e.what() << std::endl;
    }
}


void AssetManager::copyFile(const std::filesystem::path& source, const std::filesystem::path& destination) const
{
    try
    {
        if (!overWriteExisting && std::filesystem::exists(destination))
        {
            return;
        }

        std::filesystem::copy(source, destination,
                        overWriteExisting ?
                        std::filesystem::copy_options::overwrite_existing :
                        std::filesystem::copy_options::none);
    }
    catch (std::filesystem::filesystem_error& e)
    {
        throw std::runtime_error("Failed to copy file: " + source.string() + " -> "
                            + destination.string() + " - " + e.what());
    }
}



std::filesystem::path AssetManager::getRelativePath(const std::filesystem::path& sourcePath, const std::filesystem::path& basePath)
{
    try
    {
        return std::filesystem::relative(sourcePath, basePath);
    }
    catch ([[maybe_unused]] std::filesystem::filesystem_error& e)
    {
        return sourcePath.filename();
    }
}

