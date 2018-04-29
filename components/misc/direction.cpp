#define _USE_MATH_DEFINES
#include <cmath>

#include "assert.h"
#include "direction.h"
#include <array>

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

    std::pair<int32_t, int32_t> getNextPosByDir(std::pair<int32_t, int32_t> pos, Direction dir)
    {
        if (dir == Direction::invalid)
            return pos;

        int32_t x = pos.first + offsets[static_cast<size_t>(dir)][0];
        int32_t y = pos.second + offsets[static_cast<size_t>(dir)][1];

        return {x, y};
    }
}
