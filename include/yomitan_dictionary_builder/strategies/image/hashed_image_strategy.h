#ifndef HASHED_IMAGE_STRATEGY_H
#define HASHED_IMAGE_STRATEGY_H

#include <filesystem>
#include <string>
#include <unordered_map>

#include "yomitan_dictionary_builder/strategies/image/image_handling_strategy.h"

class HashedImageStrategy final : public ImageHandlingStrategy
{
public:

    explicit HashedImageStrategy(const std::string& imageMapPath);

    /**
     * Loads the image map mapping original filenames to hashed filenames
     *
     * @return True if successful
     */
    [[nodiscard]] bool loadImageMap();

protected:

    void processImageElement(const pugi::xml_node &xmlNode) const override;

private:

    /**
     * Gets the hashed image path for an image filename
     *
     * @param imageFileName Image filename
     * @return Hashed image file path if found, otherwise original path
     */
    [[nodiscard]] std::string getHashedImageFilename(const std::string &imageFileName) const;


    bool imageMapLoaded = false;
    const std::filesystem::path imageMapPath;
    std::unordered_map<std::string, std::string> imageMap;
};

#endif
