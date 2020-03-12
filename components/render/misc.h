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
        SpriteGroup(const std::string& path);
        SpriteGroup(const std::vector<Sprite> sprites) : mSprites(sprites), mAnimLength(sprites.size()) {}
        static bool canDeleteIndividualSprites();
        void destroy();

        Sprite& operator[](size_t index);
        size_t size() { return mSprites.size(); }

        size_t animLength() { return mAnimLength; }
        int32_t getWidth() const { return mWidth; }
        int32_t getHeight() const { return mHeight; }

        static void toPng(const std::string& celPath, const std::string& pngPath);
        static void toGif(const std::string& celPath, const std::string& gifPath);

    private:
        std::vector<Sprite> mSprites;
        int32_t mWidth, mHeight;
        size_t mAnimLength;
    };

    class SpriteCacheBase
    {
    public:
        virtual SpriteGroup* get(uint32_t key) = 0;
        virtual void setImmortal(uint32_t index, bool immortal) = 0;
    };
}
