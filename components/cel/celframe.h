#pragma once
#include "pal.h"
#include <misc/array2d.h>

namespace Cel
{
    class CelFrame
    {
    public:
        typedef typename Misc::Array2D<Colour>::iterator iterator;
        typedef typename Misc::Array2D<Colour>::const_iterator const_iterator;

        CelFrame() = default;
        CelFrame(int32_t x, int32_t y) : mData(x, y) {}
        CelFrame(int32_t width, int32_t height, std::vector<Colour>&& data) : mData(width, height, std::move(data)) {}
        CelFrame(const CelFrame&) = delete;
        CelFrame(CelFrame&&) = default;
        CelFrame& operator=(CelFrame&&) = default;

        const Colour& get(int32_t x, int32_t y) const { return mData.get(x, y); }

        Colour& get(int32_t x, int32_t y) { return mData.get(x, y); }

        iterator begin() { return mData.begin(); }
        const_iterator begin() const { return mData.begin(); }
        iterator end() { return mData.end(); }
        const_iterator end() const { return mData.end(); }

        int32_t width() const { return mData.width(); }
        int32_t height() const { return mData.height(); }

    public:
        Misc::Array2D<Colour> mData;
    };
}
