#ifndef FA_MISC_H
#define FA_MISC_H

#include <utility>
#include <stdint.h>
#include <stddef.h>

namespace Misc
{
    int32_t getVecDir(const std::pair<float, float>& vector);

    template <typename T>
    std::pair<float, float> getVec(const std::pair<T,T>& from, const std::pair<T,T>& to)
    {
        return std::make_pair(((float)to.first)-((float)from.first), ((float)to.second)-((float)from.second));
    }

    // Point in pixels
    struct Point
    {
      int32_t x;
      int32_t y;
      Point operator+ (const Point &v) const { return {x + v.x, y + v.y}; }
      Point operator- (const Point &v) const { return {x - v.x, y - v.y}; }
      Point operator* (double c) const { return {static_cast<int32_t> (x * c), static_cast<int32_t> (y * c)}; }
    };
}

#define UNUSED_PARAM(x) (void)(x)

#endif
