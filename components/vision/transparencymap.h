#ifndef TRANSPARENCYMAP_H
#define TRANSPARENCYMAP_H

#include <stdint.h>
#include <stddef.h>

namespace Vision
{

    class TransparencyMap
    {
    public:
        virtual ~TransparencyMap() {};

        virtual bool isTransparent(int32_t x, int32_t y) const = 0;
        virtual size_t height() const = 0;
        virtual size_t width() const = 0;
    };

}

#endif
