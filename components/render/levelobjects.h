
#pragma once

#include <boost/optional.hpp>
#include <cel/pal.h>
#include <misc/array2d.h>

namespace Render
{
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

    typedef Misc::Array2D<std::vector<LevelObject>> LevelObjects;
}
