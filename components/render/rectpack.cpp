#include "rectpack.h"

namespace Render
{
    RectPacker::RectPacker(int32_t width, int32_t height) : totalSurfaceArea(width * height) { spaces.emplace_back(new Rect{0, 0, width, height}); }

    bool RectPacker::addRect(RectPacker::Rect& box)
    {
        // Adapted / ported from: https://github.com/mapbox/potpack/blob/46615d9339e1e7c664b1ebe6c2890348dca1dc36/index.mjs

        bool foundSpace = false;

        // look through spaces backwards so that we check smaller spaces first
        for (int32_t i = int32_t(spaces.size()) - 1; i >= 0; i--)
        {
            Rect& space = *spaces[i];

            // look for empty spaces that can accommodate the current box
            if (box.w > space.w || box.h > space.h)
                continue;

            // found the space; add the box to its top-left corner
            // |-------|-------|
            // |  box  |       |
            // |_______|       |
            // |         space |
            // |_______________|
            box.x = space.x;
            box.y = space.y;

            foundSpace = true;
            usedSurfaceArea += box.w * box.h;

            if (box.w == space.w && box.h == space.h)
            {
                // space matches the box exactly; remove it
                std::unique_ptr<Rect> last = std::move(spaces.back());
                spaces.pop_back();

                if (i < int32_t(spaces.size()))
                    spaces[i] = std::move(last);
            }
            else if (box.h == space.h)
            {
                // space matches the box height; update it accordingly
                // |-------|---------------|
                // |  box  | updated space |
                // |_______|_______________|
                space.x += box.w;
                space.w -= box.w;
            }
            else if (box.w == space.w)
            {
                // space matches the box width; update it accordingly
                // |---------------|
                // |      box      |
                // |_______________|
                // | updated space |
                // |_______________|
                space.y += box.h;
                space.h -= box.h;
            }
            else
            {
                // otherwise the box splits the space into two spaces
                // |-------|-----------|
                // |  box  | new space |
                // |_______|___________|
                // | updated space     |
                // |___________________|
                spaces.emplace_back(new Rect{space.x + box.w, space.y, space.w - box.w, box.h});
                space.y += box.h;
                space.h -= box.h;
            }
            break;
        }

        return foundSpace;
    }
}