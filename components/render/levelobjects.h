#pragma once
#include <boost/container/small_vector.hpp>
#include <cel/pal.h>
#include <misc/array2d.h>
#include <misc/point.h>
#include <nonstd/optional.hpp>

namespace Render
{
    struct LevelObject
    {
        bool valid;
        int32_t spriteCacheIndex;
        int32_t spriteFrame;
        Misc::Point fractionalPos;
        nonstd::optional<Cel::Colour> hoverColor;
    };

    typedef Misc::Array2D<boost::container::small_vector<LevelObject, 2>> LevelObjects;
}
