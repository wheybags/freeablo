#pragma once
#include "rectpack.h"
#include <Image/image.h>
#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <misc/misc.h>
#include <optional>
#include <unordered_map>
#include <vector>

class Image;

namespace Render
{
    class Texture;
    class RenderInstance;
    class CommandQueue;

    class AtlasTextureEntry
    {
    public:
        AtlasTextureEntry(AtlasTextureEntry& other) = delete;

        int32_t mX = 0;
        int32_t mY = 0;
        int32_t mWidth = 0;
        int32_t mHeight = 0;

        int32_t mTrimmedOffsetX = 0;
        int32_t mTrimmedOffsetY = 0;
        int32_t mTrimmedWidth = 0;
        int32_t mTrimmedHeight = 0;

        Render::Texture* mTexture = nullptr;

    protected:
        AtlasTextureEntry() = default;
        friend class AtlasTexture;
    };

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

        std::vector<NonNullConstPtr<AtlasTextureEntry>> addCategorySprites(const std::string& category, const std::vector<LoadImageData>& images);
        void printUtilisation() const;

    private:
        const AtlasTextureEntry& addTexture(const Image& image, std::optional<Image::TrimmedData> trimmedData = std::nullopt, std::string category = "default");

    private:
        static constexpr int32_t PADDING = 1;
        static constexpr int32_t MINIMUM_ATLAS_SIZE = 1024;

        RenderInstance& mInstance;
        CommandQueue& mCommandQueue;

        std::vector<std::unique_ptr<AtlasTextureEntry>> mAtlasEntries;

        struct Layer
        {
            std::unique_ptr<Texture> texture;
            std::unique_ptr<RectPacker> rectPacker;
        };

        struct Layers
        {
            const AtlasTextureEntry* emptySpriteId = nullptr;
            std::vector<Layer> layers;
            void addLayer(RenderInstance& instance, CommandQueue& commandQueue, int32_t width, int32_t height);
        };

        std::unordered_map<std::string, Layers> mLayersByCategory;
    };
}
