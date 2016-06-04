#ifndef CEL_FRAME_H
#define CEL_FRAME_H

#include <stdint.h>
#include <vector>
#include <misc/helper2d.h>
#include "pal.h"

namespace Cel
{
    struct Colour;
    class CelFile;

    class CelFrame
    {
        public:
            size_t mWidth;
            size_t mHeight;
            
            Misc::Helper2D<const CelFrame, const Colour&> operator[] (size_t x) const;
            
        private:
            friend class CelFile;
            friend class CelDecoder;
            friend const Colour& get(size_t x, size_t y, const CelFrame& frame);
            
            std::vector<Colour> mRawImage;
    };
}

#endif
