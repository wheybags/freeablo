#define _USE_MATH_DEFINES
#include <cmath>

#include "assert.h"
#include "direction.h"
#include <array>

namespace Misc
{
    static const std::array<std::array<int32_t, 2>, 8> offsets = {{{1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}}};

    std::pair<int32_t, int32_t> getNextPosByDir(std::pair<int32_t, int32_t> pos, Direction dir)
    {
        if (dir == Direction::invalid)
            return pos;
        return {pos.first + offsets[static_cast<int32_t>(dir)][0], pos.second + offsets[static_cast<int32_t>(dir)][1]};
    }

    Direction getVecDir(const std::pair<float, float>& vector)
    {
        if (fabs(vector.first) < 1e-6f && fabs(vector.second) < 1e-6f)
            return Direction::invalid;

        auto angle = std::atan2(vector.second, vector.first);
        auto res = static_cast<int32_t>(round(angle / M_PI_4)) - 1;
        if (res < 0)
            res += 8;
        debug_assert(static_cast<int32_t>(res) >= 0 && static_cast<int32_t>(res) < 8);
        return static_cast<Direction>(res);
    }
}
