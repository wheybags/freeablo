#pragma once
#include "fixedpoint.h"
#include "simplevec2.h"
#include <cstdint>
#include <utility>

namespace Serial
{
    class Loader;
    class Saver;
}

template <typename T> class Vec2;

namespace Misc
{
    /// Enums for direction.
    /// Currently have the same numeric values as original game.
    enum class Direction8 : uint8_t
    {
        south = 0,
        south_west,
        west,
        north_west,
        north,
        north_east,
        east,
        south_east,
        none,
    };
    enum class Direction16 : uint8_t
    {
        south = 0,
        south_south_west,
        south_west,
        west_south_west,
        west,
        west_north_west,
        north_west,
        north_north_west,
        north,
        north_north_east,
        north_east,
        east_north_east,
        east,
        east_south_east,
        south_east,
        south_south_east,
        none,
    };

    using Point = Vec2<int32_t>;

    class Direction
    {
    public:
        Direction() = default;
        Direction(FixedPoint degrees);
        Direction(Direction8 dir);
        Direction(Direction16 dir);
        Direction(Serial::Loader& loader);
        static Direction fromIsometricDegrees(FixedPoint isometricDegrees);

        void save(Serial::Saver& saver) const;

        FixedPoint getDegrees() const { return mDegrees; };
        FixedPoint getIsometricDegrees();
        Direction8 getDirection8() const;
        Direction16 getDirection16() const;
        bool isNone() const;
        void adjust(FixedPoint degrees);

    private:
        void normalise();
        FixedPoint mDegrees;
    };

    Point getNextPosByDir(Point pos, Direction dir);
}
