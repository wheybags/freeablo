#include "direction.h"
#include "fixedpoint.h"
#include <array>
#include <serial/loader.h>

namespace Misc
{
    static const FixedPoint invalidDegrees = FixedPoint::maxVal();

    Direction::Direction(FixedPoint degrees)
    {
        // For simplicity orientation lines up with game directions.
        // This means 0° is south with respect to the screen.
        // Also note that positive y is south rather than north,
        // this means clockwise is positive.
        mDegrees = degrees;
        normalise();
    }

    Direction::Direction(Direction8 dir)
    {
        if (dir == Direction8::none)
            mDegrees = invalidDegrees;
        else
            mDegrees = static_cast<int32_t>(dir) * 360 / 8;
    }

    Direction::Direction(Direction16 dir)
    {
        if (dir == Direction16::none)
            mDegrees = invalidDegrees;
        else
            mDegrees = static_cast<int32_t>(dir) * 360 / 16;
    }

    Direction::Direction(Serial::Loader& loader) { mDegrees = FixedPoint::fromRawValue(loader.load<int64_t>()); }

    void Direction::save(Serial::Saver& saver) const { saver.save(mDegrees.rawValue()); }

    Direction Direction::fromIsometricDegrees(FixedPoint isometricDegrees)
    {
        // isometricDir 0° is East with respect to isometric tiles (i.e. South East).
        // normalDirSouthBased 0° is South with respect to the screen.
        // Note that positive y is south rather than north, this means clockwise is positive.

        // Adding 45° converts from isometric to normal direction,
        // subtracting 90° converts from East to South based.
        FixedPoint normalDirSouthBased = isometricDegrees + (45 - 90);

        return Direction(normalDirSouthBased);
    }

    FixedPoint Direction::getIsometricDegrees()
    {
        // Subtracting 45° converts from normal to isometric direction,
        // adding 90° converts from South to East based.
        auto isometricDegrees = mDegrees + (-45 + 90);

        return isometricDegrees;
    }

    Direction8 Direction::getDirection8() const
    {
        if (isNone())
            return Direction8::none;
        return static_cast<Direction8>((mDegrees * 8 / 360).round() % 8);
    }

    Direction16 Direction::getDirection16() const
    {
        if (isNone())
            return Direction16::none;
        return static_cast<Direction16>((mDegrees * 16 / 360).round() % 16);
    }

    bool Direction::isNone() const { return mDegrees == invalidDegrees; }

    void Direction::adjust(FixedPoint degrees)
    {
        if (isNone())
            return;
        mDegrees += degrees;
        normalise();
    }

    void Direction::normalise()
    {
        if (isNone())
            return;
        // Ensure 0 <= degrees < 360 (i.e. mDegrees %= 360).
        while (mDegrees >= 360)
        {
            mDegrees -= 360;
        }
        while (mDegrees < 0)
        {
            mDegrees += 360;
        }
    }

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
        auto dir8 = dir.getDirection8();

        if (dir8 == Direction8::none)
            return pos;

        int32_t x = pos.x + offsets[static_cast<size_t>(dir8)][0];
        int32_t y = pos.y + offsets[static_cast<size_t>(dir8)][1];

        return {x, y};
    }
}
