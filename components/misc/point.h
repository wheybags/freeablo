#pragma once

#include <cinttypes>
#include <functional>

namespace Misc
{
    struct Point
    {
        int32_t x;
        int32_t y;

        Point();
        Point(int32_t x, int32_t y);

        Point operator+(const Point& v) const { return {x + v.x, y + v.y}; }
        Point operator-(const Point& v) const { return {x - v.x, y - v.y}; }
        Point operator*(double c) const { return {static_cast<int32_t>(x * c), static_cast<int32_t>(y * c)}; }
        Point operator/(int c) const { return {x / c, y / c}; }
        bool operator==(const Point& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Point& other) const { return !(*this == other); }

        bool operator<(const Point& other) const; ///< lexicographic comparison like in std::pair
    };

} // namespace Misc

namespace std
{
    template <> struct hash<Misc::Point>
    {
        std::size_t operator()(const Misc::Point& k) const { return std::hash<int32_t>{}(k.x) ^ (hash<int32_t>{}(k.y) << 1); }
    };

} // namespace std
