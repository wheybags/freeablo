#pragma once

#include "sdl_gl_funcs.h"

#include <algorithm>
#include <map>
#include <memory>
#include <stdint.h>
#include <vector>

//#define DEBUG_ATLAS_TEXTURE

namespace rbp
{
    class SkylineBinPack;
}

namespace Render
{
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
        AtlasTexture();

        size_t addTexture(int32_t width, int32_t height, const void* data);
        void bind() const;
        void free();
        GLint getTextureWidth() const { return mTextureWidth; }
        GLint getTextureHeight() const { return mTextureHeight; }
        const AtlasTextureLookupMap& getLookupMap() const { return mLookupMap; }
        float getOccupancy() const;
        void clear();

    private:
        GLuint mTextureArrayId;
        GLint mTextureWidth;
        GLint mTextureHeight;
        GLint mTextureLayers;
        AtlasTextureLookupMap mLookupMap;
        size_t mNextTextureId = 1;
        std::vector<std::unique_ptr<rbp::SkylineBinPack>> mBinPacker;
    };
}
