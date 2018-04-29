#include "direction.h"
#include "fixedpoint.h"

class Vec2Fix
{
public:
    Vec2Fix() = default;
    Vec2Fix(FixedPoint x, FixedPoint y) : x(x), y(y) {}

    FixedPoint magnitude() const;
    void normalise();
    Misc::Direction getDirection() const;

    Vec2Fix operator-(const Vec2Fix& other) const;

    FixedPoint x;
    FixedPoint y;
};
