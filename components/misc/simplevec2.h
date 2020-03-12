#pragma once
#include "direction.h"
#include "fixedpoint.h"
#include <cstdint>
#include <ostream>
#include <serial/loader.h>
#include <vector>

namespace Misc
{
    class Direction;
}

template <typename T> class Vec2;

namespace Vec2Helper
{
    template <typename X, typename Y> void initFromOtherType(Vec2<X>& vec, Vec2<Y> other);
    template <typename X> inline void initFromOtherType(Vec2<X>& vec, Vec2<FixedPoint> other);
}

template <typename T> class Vec2
{
public:
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}
    Vec2(Serial::Loader& loader);

    template <typename OtherT> explicit Vec2(Vec2<OtherT> other) { Vec2Helper::initFromOtherType(*this, other); }

    void save(Serial::Saver& saver) const;

    Vec2<T> operator-(Vec2<T> other) const { return Vec2(x - other.x, y - other.y); }
    Vec2<T> operator+(Vec2<T> other) const { return Vec2(x + other.x, y + other.y); }

    Vec2<T> operator*(T c) const { return Vec2(x * c, y * c); }
    Vec2<T> operator/(T c) const { return Vec2(x / c, y / c); }

    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator*=(const Vec2& other);
    Vec2& operator/=(const Vec2& other);

    Vec2& operator+=(T scalar);
    Vec2& operator-=(T scalar);
    Vec2& operator*=(T scalar);
    Vec2& operator/=(T scalar);

    bool operator==(const Vec2<T>& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2<T>& other) const { return x != other.x || y != other.y; }
    bool operator<(const Vec2<T>& other) const { return x < other.x || (x <= other.x && y < other.y); }

    static Vec2 zero() { return Vec2(0, 0); }
    bool isZero() const { return x == 0 && y == 0; }
    static Vec2 invalid();
    bool isValid() const { return *this == invalid(); }

    T magnitude() const;
    T magnitudeSquared() const;
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

namespace Vec2Helper
{
    template <typename X> inline void initFromOtherType(Vec2<X>& vec, Vec2<FixedPoint> other)
    {
        vec.x = other.x.floor();
        vec.y = other.y.floor();
    }

    template <typename X, typename Y> inline void initFromOtherType(Vec2<X>& vec, Vec2<Y> other)
    {
        vec.x = other.x;
        vec.y = other.y;
    }
}

template <> void Vec2<float>::save(Serial::Saver&) const = delete;
template <> void Vec2<double>::save(Serial::Saver&) const = delete;
template <> Vec2<float>::Vec2(Serial::Loader&) = delete;
template <> Vec2<double>::Vec2(Serial::Loader&) = delete;

template <typename T> std::ostream& operator<<(std::ostream& stream, const Vec2<T>& vec) { return stream << "{" << vec.x << "," << vec.y << "}"; }

using Vec2i = Vec2<int32_t>;
using Vec2f = Vec2<float>;
using Vec2f64 = Vec2<double>;
using Vec2Fix = Vec2<FixedPoint>;

using IntRange = Vec2i;

namespace std
{
    template <> struct hash<Vec2i>
    {
        std::size_t operator()(const Vec2i& k) const { return std::hash<int32_t>{}(k.x) ^ (std::hash<int32_t>{}(k.y) << 1); }
    };
}

namespace Misc
{
    using Point = Vec2i;
    using Points = std::vector<Point>;
}
