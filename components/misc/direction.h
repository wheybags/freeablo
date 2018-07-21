#pragma once

#include "point.h"

#include <cstdint>
#include <utility>

namespace Misc
{
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

    Point getNextPosByDir(Point pos, Direction dir);
} // namespace Misc
