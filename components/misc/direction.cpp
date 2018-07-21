#define _USE_MATH_DEFINES

#include "direction.h"

#include <array>
#include <cmath>

namespace Misc
{
    static const std::array<std::array<int32_t, 2>, 8> offsets = {{
        {{1, 1}},   // s
        {{0, 1}},   // sw
        {{-1, 1}},  // w
        {{-1, 0}},  // nw
        {{-1, -1}}, // n
        {{0, -1}},  // ne
        {{1, -1}},  // e
        {{1, 0}}    // se
    }};

    Point getNextPosByDir(Point pos, Direction dir)
    {
        if (dir == Direction::invalid)
            return pos;

        int32_t x = pos.x + offsets[static_cast<size_t>(dir)][0];
        int32_t y = pos.y + offsets[static_cast<size_t>(dir)][1];

        return {x, y};
    }
} // namespace Misc
