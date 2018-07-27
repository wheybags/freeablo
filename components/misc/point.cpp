#include "point.h"

namespace Misc
{

    Point::Point(Serial::Loader& loader)
    {
        x = loader.load<int32_t>();
        y = loader.load<int32_t>();
    }

    void Point::save(Serial::Saver& saver) const
    {
        saver.save(x);
        saver.save(y);
    }
}
