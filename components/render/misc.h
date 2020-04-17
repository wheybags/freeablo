#pragma once
#include "atlastexture.h"
#include <string>
#include <vector>

struct SDL_Surface;

namespace Render
{
    typedef const AtlasTextureEntry* Sprite;

    class SpriteGroup
    {
    public:
        explicit SpriteGroup(std::vector<Sprite>&& sprites, int32_t animLength = -1) : mSprites(std::move(sprites))
        {
            if (animLength != -1)
                mAnimLength = animLength;
            else
                mAnimLength = mSprites.size();

            debug_assert(!mSprites.empty());
            mWidth = mSprites[0]->mWidth;
            mHeight = mSprites[0]->mHeight;
        }

        Sprite& operator[](size_t index) { return mSprites.at(index); }
        size_t size() { return mSprites.size(); }

        size_t animLength() { return mAnimLength; }
        int32_t getWidth() const { return mWidth; }
        int32_t getHeight() const { return mHeight; }

    private:
        std::vector<Sprite> mSprites;
        int32_t mWidth = 0, mHeight = 0;
        size_t mAnimLength;
    };
}
