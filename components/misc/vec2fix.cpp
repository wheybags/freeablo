#include "vec2fix.h"

FixedPoint Vec2Fix::magnitude() const { return FixedPoint(x * x + y * y).sqrt(); }

void Vec2Fix::normalise()
{
    if (x == 0 && y == 0)
        return;

    FixedPoint mag = magnitude();
    x = x / mag;
    y = y / mag;
}

Misc::Direction Vec2Fix::getIsometricDirection() const
{
    // isometricDir 0° is east with respect to isometric tiles (i.e. south east).
    auto isometricDir = FixedPoint::atan2_degrees(y, x);
    return Misc::Direction::fromIsometricDegrees(isometricDir);
}

Vec2Fix Vec2Fix::operator-(const Vec2Fix& other) const { return Vec2Fix(x - other.x, y - other.y); }
