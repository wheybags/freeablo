#ifndef VISIBILITYMAP_H
#define VISIBILITYMAP_H

#include <stdint.h>

namespace ShadowCaster
{

    class VisibilityMap
    {
    public:
        virtual ~VisibilityMap() {};

        virtual bool isTransparent(int32_t x, int32_t y) const = 0;
        virtual int getHeight() const = 0;
        virtual int getWidth() const = 0;
    };

}

#endif
