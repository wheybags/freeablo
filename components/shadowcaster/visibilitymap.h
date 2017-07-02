#ifndef VISIBILITYMAP_H
#define VISIBILITYMAP_H

#include <stdint.h>
#include <stddef.h>

namespace ShadowCaster
{

    class VisibilityMap
    {
    public:
        virtual ~VisibilityMap() {};

        virtual bool isTransparent(int32_t x, int32_t y) const = 0;
        virtual size_t height() const = 0;
        virtual size_t width() const = 0;
    };

}

#endif
