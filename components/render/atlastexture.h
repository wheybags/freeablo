#pragma once
#include <Image/image.h>
#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <misc/misc.h>
#include <optional>
#include <render/rectpack.h>
#include <render/texturereference.h>
#include <unordered_map>
#include <vector>

class Image;

namespace Render
{
    class RenderInstance;
    class CommandQueue;

    class AtlasTexture
    {
    public:
        explicit AtlasTexture(RenderInstance& instance, CommandQueue& commandQueue);
        ~AtlasTexture();

        struct LoadImageData
        {
            const Image& image;
            std::optional<Image::TrimmedData> trimmedData;
        };

        typedef std::unordered_map<std::string, std::vector<LoadImageData>> SpriteLoadInputMap;
        typedef std::unordered_map<std::string, std::vector<NonNullConstPtr<TextureReference>>> SpriteLoadResultMap;

        SpriteLoadResultMap loadSprites(const SpriteLoadInputMap& allSpriteData);
        void printUtilisation() const;

        void saveTexturesToCache(const filesystem::path& atlasPath) const;
        void loadTexturesFromCache(const filesystem::path& atlasPath);
        void replaceTextureEntries(std::vector<std::unique_ptr<TextureReference>>&& entries) { mAtlasEntries = std::move(entries); }

        struct Layer
        {
            std::unique_ptr<Texture> texture;
            std::unique_ptr<RectPacker> rectPacker;
        };

        struct Layers
        {
            const TextureReference* emptySpriteId = nullptr;
            std::vector<Layer> layers;
            void addLayer(RenderInstance& instance, CommandQueue& commandQueue, int32_t width, int32_t height);
        };
        const std::unordered_map<std::string, Layers>& getLayersByCategory() const { return mLayersByCategory; }

        static constexpr int32_t PADDING = 2;

    private:
        std::vector<NonNullConstPtr<TextureReference>> addCategorySprites(const std::string& category, const std::vector<LoadImageData>& images);
        const TextureReference& addTexture(const Image& image, std::optional<Image::TrimmedData> trimmedData = std::nullopt, std::string category = "default");

    private:
        static constexpr int32_t MINIMUM_ATLAS_SIZE = 1024;

        RenderInstance& mInstance;
        CommandQueue& mCommandQueue;

        std::vector<std::unique_ptr<TextureReference>> mAtlasEntries;

        std::unordered_map<std::string, Layers> mLayersByCategory;
    };
}
