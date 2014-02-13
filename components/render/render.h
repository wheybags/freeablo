#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include <vector>
#include <string>

namespace Level
{
    class DunFile;
    class TilFile;
    class MinFile;
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

    void setLevel(const std::string& tilesetPath, const Level::DunFile& dun, const Level::TilFile& til, const Level::MinFile& min);
    void drawLevel(int32_t x1, int32_t y1, int32_t x2, int32_t y2, size_t dist);

    void clear();
}

#endif
