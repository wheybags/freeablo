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
        size_t spriteCacheIndex;
        size_t spriteFrame;
        int32_t x2;
        int32_t y2;
        int32_t dist;
    };

    class LevelObjects
    {
        public:
            void resize(size_t x, size_t y);

            Misc::Helper2D<LevelObjects, LevelObject&> operator[] (size_t x);

            size_t width();
            size_t height();
        
        private:
            std::vector<LevelObject> mData;
            size_t mWidth;
            size_t mHeight;

            friend LevelObject& get(size_t x, size_t y, LevelObjects& obj);
    };
}

#endif
