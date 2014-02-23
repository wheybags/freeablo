#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include <vector>
#include <string>

namespace Level
{
    class Dun;
    class TileSet;
    class Min;
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

        private:
            std::vector<Sprite> mSprites;
    };

    void setLevel(const std::string& tilesetPath, const Level::Dun& dun, const Level::TileSet& til, const Level::Min& min);
    void drawLevel(int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);
    void drawAt(const Sprite& sprite, int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    void clear();
}

#endif
