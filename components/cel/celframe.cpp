#include <cel/celframe.h>
#include <cstring>

namespace Cel
{
    void CelFrame::blitTo(CelFrame& other, int32_t destOffsetX, int32_t destOffsetY)
    {
        release_assert(destOffsetX >= 0 && destOffsetY >= 0);
        release_assert(destOffsetX + width() <= other.width() && destOffsetY + height() <= other.height());

        for (int32_t line = 0; line < height(); line++)
        {
            Colour* src = &get(0, line);
            Colour* dest = &other.get(destOffsetX, line + destOffsetY);

            memcpy(dest, src, sizeof(Colour) * width());
        }
    }
}