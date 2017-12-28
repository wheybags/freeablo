
#pragma once

#include <misc/helper2d.h>
#include <stdint.h>
#include <vector>

namespace Cel
{
    struct Colour;
    class CelFile;

    class CelFrame
    {
    public:
        int32_t mWidth;
        int32_t mHeight;

        Misc::Helper2D<const CelFrame, const Colour&> operator[](int32_t x) const;

    private:
        friend class CelFile;
        friend class CelDecoder;
        friend const Colour& get(int32_t x, int32_t y, const CelFrame& frame);

        std::vector<Colour> mRawImage;
    };
}
