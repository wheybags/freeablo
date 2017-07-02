#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

#include <stdint.h>
#include <set>

#include "visibilitymap.h"

namespace ShadowCaster
{

    class Scanner
    {
    public:
        Scanner(
            const VisibilityMap* visibilityMap,
            std::pair<int32_t, int32_t> startPos,
            int lightingRadius = 10 * 10
        ): mVisibilityMap(visibilityMap),
           mStartPos(startPos),
           mLightingRadius(lightingRadius) {}

        std::set<std::pair<int32_t, int32_t>> getVisibleTiles();

    private:
        const VisibilityMap* mVisibilityMap;
        std::pair<int32_t, int32_t> mStartPos;
        int mLightingRadius;

        std::set<std::pair<int32_t, int32_t>> mVisibleTiles;

        void scan(int depth, int octant, float startSlope, float endSlope);
        float getSlope(float x1, float y1, bool invert);
        int getDistance(int x1, int y1);
        inline bool isTransparent(int32_t x, int32_t y) {
            return mVisibilityMap->isTransparent(x, y);
        }
    };

}

#endif
