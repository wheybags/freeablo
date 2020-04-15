#pragma once
#include <string>
#include <vector>

struct SDL_Surface;

namespace Render
{
    typedef void* Sprite;
    typedef SDL_Surface* FASurface;

    class SpriteGroup
    {
    public:
        explicit SpriteGroup(std::vector<Sprite>&& sprites, int32_t animLength = -1);

        Sprite& operator[](size_t index);
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
