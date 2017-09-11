#include "render.h"

#include <misc/helper2d.h>

#ifndef LEVEL_OBJ_H
#define LEVEL_OBJ_H

namespace Render
{
    class SpriteGroup;

    struct LevelObject
    {
        bool valid;
        int32_t spriteCacheIndex;
        int32_t spriteFrame;
        int32_t x2;
        int32_t y2;
        int32_t dist;
        boost::optional<Cel::Colour> hoverColor;
    };

    class LevelObjects
    {
        public:
            void resize(int32_t x, int32_t y);

            Misc::Helper2D<LevelObjects, std::vector<LevelObject>&> operator[] (int32_t x);

            int32_t width();
            int32_t height();

        private:
            std::vector<std::vector<LevelObject>> mData;
            int32_t mWidth;
            int32_t mHeight;

            friend std::vector<LevelObject>& get(int32_t x, int32_t y, LevelObjects& obj);
    };
}

#endif
