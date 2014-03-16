#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include <vector>
#include <string>

namespace Level
{
    class Level;
}

namespace Render
{
    void init(); 
    void draw();

    typedef void* Sprite;
    void drawAt(const Sprite& sprite, size_t x, size_t y); 

    class SpriteGroup
    {
        public:
            SpriteGroup(const std::string& path);
            ~SpriteGroup();
            
            Sprite& operator[](size_t index)
            {
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

        private:
            std::vector<Sprite> mSprites;
            size_t mAnimLength;
    };

    struct RenderLevel
    {
        public:
            ~RenderLevel();

        private:
            Sprite levelSprite;
            int32_t levelHeight;
            int32_t levelWidth;
            int32_t levelX;
            int32_t levelY;

            friend RenderLevel* setLevel(const Level::Level& level);
            friend void drawLevel(RenderLevel* level, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
            friend void drawAt(RenderLevel* level, const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
    };

    RenderLevel* setLevel(const Level::Level& level);
    void drawLevel(RenderLevel* level, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
    void drawAt(RenderLevel* level, const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    void clear();
}

#endif
