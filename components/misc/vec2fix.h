#include "direction.h"
#include "fixedpoint.h"

class Vec2Fix
{
public:
    Vec2Fix() = default;
    Vec2Fix(FixedPoint x, FixedPoint y) : x(x), y(y) {}

    void normalise();
    Misc::Direction getDirection() const;

    FixedPoint x;
    FixedPoint y;
};
