#include "level.h"

#include <iostream>

namespace Level
{
    Level::Level(const Dun& dun, const std::string& tilPath, const std::string& minPath,
        const std::string& solPath, const std::string& tileSetPath, const std::pair<size_t,size_t>& downStairs,
        const std::pair<size_t,size_t>& upStairs, std::map<size_t, size_t> doorMap, size_t previous, size_t next):
        mDun(dun), mTil(tilPath), mMin(minPath), mSol(solPath), mTileSetPath(tileSetPath),

        mTilPath(tilPath), mMinPath(minPath), mSolPath(solPath), mDoorMap(doorMap), mUpStairs(upStairs), mDownStairs(downStairs),
        mPrevious(previous), mNext(next){}

    std::vector<int16_t> Level::mEmpty(16);

    bool Level::isStairs(size_t x, size_t y) const
    {
        if(mDownStairs.first == x && mDownStairs.second == y)
            return true;
        if(mUpStairs.first == x && mUpStairs.second == y)
            return true;

        return false;
    }

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
            return MinPillar(Level::mEmpty, 0, -1);

        size_t minIndex = level.mTil[dunIndex][tilIndex];
        
        return MinPillar(level.mMin[minIndex], level.mSol.passable(minIndex), minIndex);
    }

    Misc::Helper2D<const Level, const MinPillar> Level::operator[] (size_t x) const
    {
        return Misc::Helper2D<const Level, const MinPillar>(*this, x, get);
    }
            
    void Level::activate(size_t x, size_t y)
    {
        size_t xDunIndex = x;
        if((xDunIndex % 2) != 0)
            xDunIndex--;
        xDunIndex /= 2;

        size_t yDunIndex = y;
        if((yDunIndex % 2) != 0)
            yDunIndex--;
        yDunIndex /= 2;

        size_t index = mDun[xDunIndex][yDunIndex]; 
        
        // open doors when clicked on
        if(mDoorMap.find(index) != mDoorMap.end())
            mDun[xDunIndex][yDunIndex] = mDoorMap[index];
    }
    
    size_t Level::minSize() const
    {
        return mMin.size();
    }

    const MinPillar Level::minPillar(int32_t i) const
    {
        return MinPillar(mMin[i], mSol.passable(i), i);
    }

    size_t Level::width() const
    {
        return mDun.width()*2;
    }

    size_t Level::height() const
    {
        return mDun.height()*2;
    }

    const std::pair<size_t,size_t>& Level::upStairsPos() const
    {
        return mUpStairs;
    }

    const std::pair<size_t,size_t>& Level::downStairsPos() const
    {
        return mDownStairs;
    }

    const std::string& Level::getTileSetPath() const
    {
        return mTileSetPath;
    }

    const std::string& Level::getMinPath() const
    {
        return mMinPath;
    }

    MinPillar::MinPillar(const std::vector<int16_t>& data, bool passable, int32_t index): mData(data), mPassable(passable), mIndex(index) {}

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

    int32_t MinPillar::index() const
    {
        return mIndex;
    }
}
