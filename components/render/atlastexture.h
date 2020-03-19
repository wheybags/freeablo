#pragma once
#include <algorithm>
#include <cstdint>
#include <glad/glad.h>
#include <map>
#include <memory>
#include <vector>
//#define DEBUG_ATLAS_TEXTURE

namespace rbp
{
    class SkylineBinPack;
}

namespace Render
{
    class Texture;
    class RenderInstance;
    class CommandQueue;

    class AtlasTextureEntry
    {
    public:
        AtlasTextureEntry() = default;
        AtlasTextureEntry(int32_t x, int32_t y, int32_t layer, int32_t width, int32_t height) : mX(x), mY(y), mLayer(layer), mWidth(width), mHeight(height) {}

        int32_t mX = 0, mY = 0, mLayer = 0, mWidth = 0, mHeight = 0;
    };

    typedef std::map<size_t, AtlasTextureEntry> AtlasTextureLookupMap;

    class AtlasTexture
    {
    public:
        explicit AtlasTexture(RenderInstance& instance, CommandQueue& commandQueue);
        ~AtlasTexture();

        size_t addTexture(int32_t width, int32_t height, const void* data);
        const AtlasTextureLookupMap& getLookupMap() const { return mLookupMap; }
        float getOccupancy() const;
        void clear(CommandQueue& commandQueue);

        Texture& getTextureArray() { return *mTextureArray; }

    private:
        static constexpr int32_t PADDING = 1;

    private:
        RenderInstance& mInstance;
        std::unique_ptr<Texture> mTextureArray;

        AtlasTextureLookupMap mLookupMap;
        size_t mNextTextureId = 1;
        std::vector<std::unique_ptr<rbp::SkylineBinPack>> mBinPacker;
    };
}
