#pragma once

#include <cstdint>
#include <utility>

namespace Misc
{
    class Point;

    /// Enum for direction.
    /// Currently has the same numeric values as original game.
    enum class Direction : uint8_t
    {
        south = 0,
        south_west,
        west,
        north_west,
        north,
        north_east,
        east,
        south_east,
        invalid,
    };

    Misc::Point getNextPosByDir(Misc::Point pos, Direction dir);
}
