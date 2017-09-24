#pragma once

#include <assert.h>
#include <string>

struct SDL_Cursor;
struct SDL_Surface;


namespace Render
{
    typedef void* Sprite;
    typedef SDL_Cursor * FACursor;
    typedef SDL_Surface * FASurface;


    class SpriteGroup
    {
        public:
            SpriteGroup(const std::string& path);
            SpriteGroup(const std::vector<Sprite> sprites): mSprites(sprites), mAnimLength(sprites.size()) {}
            void destroy();

            Sprite& operator[](size_t index)
            {
                #ifndef NDEBUG
                    assert(index < mSprites.size());
                #endif
                return mSprites[index];
            }

            size_t size()
            {
                return mSprites.size();
            }

            size_t animLength()
            {
                return mAnimLength;
            }

            static void toPng(const std::string& celPath, const std::string& pngPath);


        private:
            std::vector<Sprite> mSprites;
            size_t mAnimLength;
    };

    class SpriteCacheBase
    {
        public:
            virtual SpriteGroup* get(uint32_t key) = 0;
            virtual void setImmortal(uint32_t index, bool immortal) = 0;
    };
}
