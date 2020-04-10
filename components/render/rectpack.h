#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace Render
{
    class RectPacker
    {
    public:
        struct Rect
        {
            int32_t x, y, w, h;
        };

        RectPacker(int32_t width, int32_t height);

        // NOTE! sort rects by height (descending) before calling this, or utilisation will be terrible
        bool addRect(Rect& rect);
        float utilisation() const { return float(usedSurfaceArea) / totalSurfaceArea; }

    private:
        std::vector<std::unique_ptr<Rect>> spaces;
        int32_t usedSurfaceArea = 0;
        int32_t totalSurfaceArea = 0;
    };
}