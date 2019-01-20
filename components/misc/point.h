#pragma once

#include <cinttypes>
#include <functional>
#include <serial/loader.h>
#include <vector>

namespace Misc
{
    struct Point
    {
        int32_t x = 0;
        int32_t y = 0;

        Point() = default;
        Point(int32_t x, int32_t y) : x(x), y(y) {}

        explicit Point(Serial::Loader& loader);
        void save(Serial::Saver& saver) const;

        inline Point operator+(const Point& v) const { return {x + v.x, y + v.y}; }
        inline Point operator-(const Point& v) const { return {x - v.x, y - v.y}; }
        inline Point operator*(double c) const { return {static_cast<int32_t>(x * c), static_cast<int32_t>(y * c)}; }
        inline Point operator/(int c) const { return {x / c, y / c}; }
        bool operator==(const Point& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Point& other) const { return !(*this == other); }

        bool operator<(const Point& other) const
        {
            // lexicographic comparison like in std::pair
            return x < other.x || (x <= other.x && y < other.y);
        }

        static Point invalid() { return Point{std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()}; }
        bool isValid() const { return *this != Point::invalid(); }
    };

    using Points = std::vector<Point>;
}

namespace std
{
    template <> struct hash<Misc::Point>
    {
        std::size_t operator()(const Misc::Point& k) const { return std::hash<int32_t>{}(k.x) ^ (hash<int32_t>{}(k.y) << 1); }
    };
}
