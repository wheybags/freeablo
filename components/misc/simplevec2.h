#pragma once
#include "direction.h"
#include "fixedpoint.h"
#include <cstdint>
#include <ostream>

template <typename T> class Vec2
{
public:
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}

    Vec2<T> operator-(Vec2<T> other) const { return Vec2(x - other.x, y - other.y); }
    Vec2<T> operator+(Vec2<T> other) const { return Vec2(x + other.x, y + other.y); }

    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);

    bool operator==(const Vec2<T>& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2<T>& other) const { return x != other.x || y != other.y; }

    bool isZero() { return x == 0 && y == 0; }

    T magnitude() const;
    void normalise();

    /// NOTE: this function returns a direction in isometric space,
    /// where positive x extends south east from the origin, and positive y south west
    Misc::Direction getDirection() const;

    // this trick lets us refer to the vector by x/y, or u/v, etc while still being the same underneath
    union
    {
        T x;
        T u;
        T start;
        T min;
    };

    union
    {
        T y;
        T v;
        T end;
        T max;
    };
};

template <typename T> std::ostream& operator<<(std::ostream& stream, const Vec2<T>& vec) { return stream << "{" << vec.x << "," << vec.y << "}"; }

using Vec2i = Vec2<int32_t>;
using Vec2f = Vec2<float>;
using Vec2f64 = Vec2<double>;
using Vec2Fix = Vec2<FixedPoint>;

using IntRange = Vec2i;
