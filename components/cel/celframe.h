#pragma once
#include "pal.h"
#include <misc/array2d.h>

namespace Cel
{
    /// This class just wraps Misc::Array2D with get() functions that reverse the y-axis.
    /// This is because diablo cel files are encoded bottom-up.
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

        const Colour& get(int32_t x, int32_t y) const { return mData.get(x, mData.height() - 1 - y); }

        Colour& get(int32_t x, int32_t y) { return mData.get(x, mData.height() - 1 - y); }

        iterator begin() { return mData.begin(); }
        const_iterator begin() const { return mData.begin(); }
        iterator end() { return mData.end(); }
        const_iterator end() const { return mData.end(); }

        int32_t width() const { return mData.width(); }
        int32_t height() const { return mData.height(); }

        std::vector<Colour>& getFlatVector() { return mData.getFlatVector(); }

    private:
        Misc::Array2D<Colour> mData;
    };
}
