#include "celframe.h"

#include "pal.h"

namespace Cel
{
    const Colour& get(size_t x, size_t y, const CelFrame& frame)
    {
        return frame.mRawImage[x + (frame.mHeight-1-y)*frame.mWidth];
    }
     
    Misc::Helper2D<const CelFrame, const Colour&> CelFrame::operator[] (size_t x) const
    {
        return Misc::Helper2D<const CelFrame, const Colour&>(*this, x, get);
    }
}
