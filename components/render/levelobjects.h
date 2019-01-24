#pragma once
#include <boost/container/small_vector.hpp>
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
        int32_t fractionalPosX;
        int32_t fractionalPosY;
        boost::optional<Cel::Colour> hoverColor;
    };

    typedef Misc::Array2D<boost::container::small_vector<LevelObject, 2>> LevelObjects;
}
