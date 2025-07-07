#include "yomitan_dictionary_builder/strategies/image/hashed_image_strategy.h"
#include "yomitan_dictionary_builder/utils/file_utils.h"


HashedImageStrategy::HashedImageStrategy(const std::string &imageMapPath) : imageMapPath(imageMapPath)
{
    if (imageMapPath.empty())
    {
        std::cerr << "Image map path is empty" << std::endl;
    }

    if (!std::filesystem::exists(imageMapPath) || !imageMapPath.ends_with(".json"))
    {
        std::cerr << "Image map does not exist: " << imageMapPath << std::endl;
    }

    if (!this->loadImageMap())
    {
        std::cerr << "Failed to load image map" << std::endl;
    }
}



bool HashedImageStrategy::loadImageMap()
{
    const auto json = FileUtils::readFile(this->imageMapPath);
    if (!json.has_value())
        return false;

    try
    {
        if (const auto ec = glz::read_json(this->imageMap, json.value()))
        {
            std::cerr << "Error reading tag map: " << glz::format_error(ec, json.value()) << std::endl;
            return false;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error reading image map: " << e.what() << std::endl;
        return false;
    }

    this->imageMapLoaded = true;
    return true;
}


void HashedImageStrategy::processImageElement(const pugi::xml_node& xmlNode) const
{
    if (!this->imageMapLoaded)
        return;

    if (const auto srcPath = this->getImagePath(xmlNode); srcPath.has_value())
    {
        const auto dirName = srcPath.value().parent_path();
        const auto originalFileName = srcPath.value().filename().string();

        const auto hashedFileName = this->getHashedImageFilename(originalFileName);
        xmlNode.attribute("src") = (dirName / hashedFileName).string();
    }
}


std::string HashedImageStrategy::getHashedImageFilename(const std::string &imageFileName) const
{
    if (const auto hash = this->imageMap.find(imageFileName); hash != this->imageMap.end())
        return hash->second;

    std::cerr << "Image filename not found: " << imageFileName << std::endl;
    return imageFileName;
}
