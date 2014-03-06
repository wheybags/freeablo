#include "level.h"

#include <iostream>

namespace Level
{
    Level::Level(const std::string& dunPath, const std::string& tilPath, const std::string& minPath, const std::string& solPath):
        mDun(dunPath), mTil(tilPath), mMin(minPath), mSol(solPath) {} 
     
    Level::Level(const Dun& dun, const std::string& tilPath, const std::string& minPath, const std::string& solPath):
        mDun(dun), mTil(tilPath), mMin(minPath), mSol(solPath) {}

    std::vector<int16_t> Level::mEmpty(16);

    const MinPillar get(size_t x, size_t y, const Level& level)
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
            return MinPillar(Level::mEmpty, 0);

        size_t minIndex = level.mTil[dunIndex][tilIndex];
        
        return MinPillar(level.mMin[minIndex], level.mSol.passable(minIndex));
    }

    Misc::Helper2D<const Level, const MinPillar> Level::operator[] (size_t x) const
    {
        return Misc::Helper2D<const Level, const MinPillar>(*this, x, get);
    }

    size_t Level::width() const
    {
        return mDun.width()*2;
    }

    size_t Level::height() const
    {
        return mDun.height()*2;
    }

    MinPillar::MinPillar(const std::vector<int16_t>& data, bool passable): mData(data), mPassable(passable) {}

    size_t MinPillar::size() const
    {
        return mData.size();
    }

    int16_t MinPillar::operator[] (size_t index) const
    {
        return mData[index];
    }

    bool MinPillar::passable() const
    {
        return mPassable;
    }
    
}
