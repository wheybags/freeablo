#include "scanner.h"
#include <iostream>
#include <cmath>

namespace Vision
{

    std::set<std::pair<int32_t, int32_t>> Scanner::getVisibleTiles()
    {
        mVisibleTiles = {};
        scan(1, 1, 1.0, 0.0);
        scan(1, 2, 1.0, 0.0);
        scan(1, 3, 1.0, 0.0);
        scan(1, 4, 1.0, 0.0);
        scan(1, 5, 1.0, 0.0);
        scan(1, 6, 1.0, 0.0);
        scan(1, 7, 1.0, 0.0);
        scan(1, 8, 1.0, 0.0);
        return mVisibleTiles;
    }

    inline double antitrunc(double x)
    {
        return x < 0 ? floor(x) : ceil(x);
    }

    void Scanner::scan(int depth, int octant, float startSlope, float endSlope)
    {
        auto x = 0;
        auto y = 0;

        switch (octant) {
        case 1:
            y = mStartPos.second - depth;
            if (y < 0) return;

            x = mStartPos.first - (int)antitrunc(startSlope * (double)depth);
            if (x < 0) x = 0;

            while (getSlope(x, y, false) >= endSlope) {
                if (!isTransparent(x, y)) {
                    if (x - 1 >= 0 && isTransparent(x - 1, y)) {
                        auto newEndSlope = getSlope(x - 0.5, y + 0.5, false);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (x - 1 >= 0 && !isTransparent(x - 1, y) && x - 1 > mStartPos.first - depth) {
                        startSlope = getSlope(x - 0.5, y - 0.5, false);
                    }
                    mVisibleTiles.insert({x, y});
                }
                x++;
            }
            x--;
            break;
        case 2:
            y = mStartPos.second - depth;
            if (y < 0) return;

            x = mStartPos.first + (int)antitrunc(startSlope * (double)depth);
            if (x >= (int)mTransparencyMap->width()) x = mTransparencyMap->width() - 1;

            while (getSlope(x, y, false) <= endSlope) {
                if (!isTransparent(x, y)) {
                    if (x + 1 < (int)mTransparencyMap->width() && isTransparent(x + 1, y)) {
                        auto newEndSlope = getSlope(x + 0.5, y + 0.5, false);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (x + 1 < (int)mTransparencyMap->width() && !isTransparent(x + 1, y) && x + 1 < mStartPos.first + depth) {
                        startSlope = -getSlope(x + 0.5, y - 0.5, false);
                    }
                    mVisibleTiles.insert({x, y});
                }
                x--;
            }
            x++;
            break;
        case 3:
            x = mStartPos.first + depth;
            if (x >= (int)mTransparencyMap->width()) return;

            y = mStartPos.second - (int)antitrunc(startSlope * (double)depth);
            if (y < 0) y = 0;

            while (getSlope(x, y, true) <= endSlope) {
                if (!isTransparent(x, y)) {
                    if (y - 1 >= 0 && isTransparent(x, y - 1) && y - 1 > mStartPos.second - depth) {
                        auto newEndSlope = getSlope(x - 0.5, y - 0.5, true);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (y - 1 >= 0 && !isTransparent(x, y - 1) && y - 1 >= mStartPos.second - depth) {
                        startSlope = -getSlope(x + 0.5, y - 0.5, true);
                    }
                    mVisibleTiles.insert({x, y});
                }
                y++;
            }
            y--;
            break;
        case 4:
            x = mStartPos.first + depth;
            if (x >= (int)mTransparencyMap->width()) return;

            y = mStartPos.second + (int)antitrunc(startSlope * (double)depth);
            if (y >= (int)mTransparencyMap->height()) y = mTransparencyMap->height() - 1;

            while (getSlope(x, y, true) >= endSlope) {
                if (!isTransparent(x, y)) {
                    if (y + 1 < (int)mTransparencyMap->height() && isTransparent(x, y + 1)) {
                        auto newEndSlope = getSlope(x - 0.5, y + 0.5, true);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (y + 1 < (int)mTransparencyMap->height() && !isTransparent(x, y + 1) && y + 1 < mStartPos.second + depth) {
                        startSlope = getSlope(x + 0.5, y + 0.5, true);
                    }
                    mVisibleTiles.insert({x, y});
                }
                y--;
            }
            y++;
            break;
        case 5:
            y = mStartPos.second + depth;
            if (y >= (int)mTransparencyMap->height()) return;

            x = mStartPos.first + (int)antitrunc(startSlope * (double)depth);
            if (x >= (int)mTransparencyMap->width()) x = mTransparencyMap->width() - 1;

            while (getSlope(x, y, false) >= endSlope) {
                if (!isTransparent(x, y)) {
                    if (x + 1 < (int)mTransparencyMap->width() && isTransparent(x + 1, y)) {
                        auto newEndSlope = getSlope(x + 0.5, y - 0.5, false);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (x + 1 < (int)mTransparencyMap->width() && !isTransparent(x + 1, y) && x + 1 < mStartPos.first + depth) {
                        startSlope = getSlope(x + 0.5, y + 0.5, false);
                    }
                    mVisibleTiles.insert({x, y});
                }
                x--;
            }
            x++;
            break;
        case 6:
            y = mStartPos.second + depth;
            if (y >= (int)mTransparencyMap->height()) return;

            x = mStartPos.first - (int)antitrunc(startSlope * (double)depth);
            if (x < 0) x = 0;

            while (getSlope(x, y, false) <= endSlope) {
                if (!isTransparent(x, y)) {
                    if (x - 1 >= 0 && isTransparent(x - 1, y)) {
                        auto newEndSlope = getSlope(x - 0.5, y - 0.5, false);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (x - 1 >= 0 && !isTransparent(x - 1, y) && x - 1 > mStartPos.first - depth) {
                        startSlope = -getSlope(x - 0.5, y + 0.5, false);
                    }
                    mVisibleTiles.insert({x, y});
                }
                x++;
            }
            x--;
            break;
        case 7:
            x = mStartPos.first - depth;
            if (x < 0) return;

            y = mStartPos.second + (int)antitrunc(startSlope * (double)depth);
            if (y >= (int)mTransparencyMap->height()) y = mTransparencyMap->height() - 1;

            while (getSlope(x, y, true) <= endSlope) {
                if (!isTransparent(x, y)) {
                    if (y + 1 < (int)mTransparencyMap->height() && isTransparent(x, y + 1)) {
                        auto newEndSlope = getSlope(x + 0.5, y + 0.5, true);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (y + 1 < (int)mTransparencyMap->height() && !isTransparent(x, y + 1) && y + 1 < mStartPos.second + depth) {
                        startSlope = -getSlope(x - 0.5, y + 0.5, true);
                    }
                    mVisibleTiles.insert({x, y});
                }
                y--;
            }
            y++;
            break;
        case 8:
            x = mStartPos.first - depth;
            if (x < 0) return;

            y = mStartPos.second - (int)antitrunc(startSlope * (double)depth);
            if (y < 0) y = 0;

            while (getSlope(x, y, true) >= endSlope) {
                if (!isTransparent(x, y)) {
                    if (y - 1 >= 0 && isTransparent(x, y - 1)) {
                        auto newEndSlope = getSlope(x + 0.5, y - 0.5, true);
                        scan(depth + 1, octant, startSlope, newEndSlope);
                    }
                } else {
                    if (y - 1 >= 0 && !isTransparent(x, y - 1) && y - 1 > mStartPos.second - depth) {
                        startSlope = getSlope(x - 0.5, y - 0.5, true);
                    }
                    mVisibleTiles.insert({x, y});
                }
                y++;
            }
            y--;
            break;
        }

        if (x < 0) x = 0;
        if (x >= (int)mTransparencyMap->width()) x = mTransparencyMap->width() - 1;

        if (y < 0) y = 0;
        if (y >= (int)mTransparencyMap->height()) y = mTransparencyMap->height() - 1;

        if (depth < mLightingRadius && isTransparent(x, y)) {
            scan(depth + 1, octant, startSlope, endSlope);
        }

    }


    float Scanner::getSlope(float x1, float y1, bool invert)
    {
        auto x2 = mStartPos.first;
        auto y2 = mStartPos.second;

        if (invert) {
            return (y1 - y2) / (x1 - x2);
        }

        return (x1 - x2) / (y1 - y2);
    }


    int Scanner::getDistance(int x1, int y1)
    {
        auto x2 = mStartPos.first;
        auto y2 = mStartPos.second;

        return ((x1 - x2)*(x1 - x2)) + ((y1 - y2) * (y1 - y2));
    }


}
