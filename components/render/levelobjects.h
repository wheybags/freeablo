#pragma once
#include <cel/pal.h>
#include <misc/array2d.h>
#include <misc/simplevec2.h>
#include <optional>
#include <vector>

namespace Render
{
    struct LevelObject
    {
        bool valid;
        int32_t spriteCacheIndex;
        int32_t spriteFrame;
        Vec2Fix fractionalPos;
        std::optional<Cel::Colour> hoverColor;
    };

    typedef Misc::Array2D<std::vector<LevelObject>> LevelObjects; // TODO: get a custom small vector class + use it here
}
