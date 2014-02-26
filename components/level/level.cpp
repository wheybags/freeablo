#include "level.h"

#include <iostream>

namespace Level
{
    Level::Level(const std::string& dunPath, const std::string& tilPath, const std::string& minPath):
        mDun(dunPath), mTil(tilPath), mMin(minPath) {} 
     
    Level::Level(const Dun& dun, const std::string& tilPath, const std::string& minPath):
        mDun(dun), mTil(tilPath), mMin(minPath) {}
    
    MinPillar Level::mEmpty(std::vector<int16_t>(16));

    const MinPillar& get(size_t x, size_t y, const Level& level)
    {
        size_t xDunIndex = x;
        size_t xTilIndex = 0;
        if((xDunIndex % 2) != 0)
        {
            xDunIndex--;
            xTilIndex = 1;
        }
        xDunIndex /= 2;

        size_t yDunIndex = y;
        size_t yTilIndex = 0;
        if((yDunIndex % 2) != 0)
        {
            yDunIndex--;
            yTilIndex = 1;
        }
        yDunIndex /= 2;

        size_t tilIndex;
        
        if(xTilIndex)
        {
            if(yTilIndex)
                tilIndex = 3; // bottom
            else
                tilIndex = 1; // left
        }
        else
        {
            if(yTilIndex)
                tilIndex = 2; // right
            else
                tilIndex = 0; // top
        }

        int32_t dunIndex = level.mDun[xDunIndex][yDunIndex]-1;

        if(dunIndex == -1)
            return Level::mEmpty;
        
        return level.mMin[level.mTil[dunIndex][tilIndex]];
    }

    Misc::Helper2D<const Level, const MinPillar&> Level::operator[] (size_t x) const
    {
        return Misc::Helper2D<const Level, const MinPillar&>(*this, x, get);
    }

    size_t Level::width() const
    {
        return mDun.width()*2;
    }

    size_t Level::height() const
    {
        return mDun.height()*2;
    }
}
