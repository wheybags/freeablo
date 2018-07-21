#include "point.h"

namespace Misc
{

    Point::Point() : Point(0, 0) {}

    Point::Point(int32_t x, int32_t y) : x(x), y(y) {}

    bool Point::operator<(const Point& other) const { return x < other.x || (x <= other.x && y < other.y); }

} // namespace Misc
