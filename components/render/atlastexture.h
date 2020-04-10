#pragma once
#include "rectpack.h"
#include <Image/image.h>
#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
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
        int32_t mX = 0;
        int32_t mY = 0;
        int32_t mLayer = 0;
        int32_t mWidth = 0;
        int32_t mHeight = 0;

        int32_t mTrimmedOffsetX = 0;
        int32_t mTrimmedOffsetY = 0;
        int32_t mTrimmedWidth = 0;
        int32_t mTrimmedHeight = 0;
    };

    typedef std::map<size_t, AtlasTextureEntry> AtlasTextureLookupMap;

    class AtlasTexture
    {
    public:
        explicit AtlasTexture(RenderInstance& instance, CommandQueue& commandQueue);
        ~AtlasTexture();

        size_t addTexture(const Image& image, bool trim = true, std::optional<Image::TrimmedData> trimmedData = std::nullopt);
        const AtlasTextureLookupMap& getLookupMap() const { return mLookupMap; }
        void printUtilisation() const;
        void clear(CommandQueue& commandQueue);

        Texture& getTextureArray() { return *mTextureArray; }

    private:
        static constexpr int32_t PADDING = 1;

    private:
        RenderInstance& mInstance;
        std::unique_ptr<Texture> mTextureArray;

        size_t mEmptySpriteId = 0;

        AtlasTextureLookupMap mLookupMap;
        size_t mNextTextureId = 1;

        std::vector<std::unique_ptr<RectPacker>> mRectPacker;
    };
}
