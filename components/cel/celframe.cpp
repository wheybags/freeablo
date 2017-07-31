#include "celframe.h"

namespace Cel
{
    const Colour& get(int32_t x, int32_t y, const CelFrame& frame)
    {
        return frame.mRawImage[x + (frame.mHeight-1-y)*frame.mWidth];
    }
     
    Misc::Helper2D<const CelFrame, const Colour&> CelFrame::operator[] (int32_t x) const
    {
        return Misc::Helper2D<const CelFrame, const Colour&>(*this, x, get);
    }
}
