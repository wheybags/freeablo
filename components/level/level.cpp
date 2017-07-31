#include "level.h"

#include <iostream>

namespace Level
{
    Level::Level(const Dun& dun, const std::string& tilPath, const std::string& minPath,
        const std::string& solPath, const std::string& tileSetPath, const std::pair<int32_t,int32_t>& downStairs,
        const std::pair<int32_t,int32_t>& upStairs, std::map<int32_t, int32_t> doorMap, int32_t previous, int32_t next):
        mDun(dun), mTil(tilPath), mMin(minPath), mSol(solPath), mTileSetPath(tileSetPath),

        mTilPath(tilPath), mMinPath(minPath), mSolPath(solPath), mDoorMap(doorMap), mUpStairs(upStairs), mDownStairs(downStairs),
        mPrevious(previous), mNext(next){}

    std::vector<int16_t> Level::mEmpty(16);

    bool Level::isStairs(int32_t x, int32_t y) const
    {
        if(mDownStairs.first == x && mDownStairs.second == y)
            return true;
        if(mUpStairs.first == x && mUpStairs.second == y)
            return true;

        return false;
    }

    const MinPillar get(int32_t x, int32_t y, const Level& level)
    {
        int32_t xDunIndex = x;
        int32_t xTilIndex = 0;
        if((xDunIndex % 2) != 0)
        {
            xDunIndex--;
            xTilIndex = 1;
        }
        xDunIndex /= 2;

        int32_t yDunIndex = y;
        int32_t yTilIndex = 0;
        if((yDunIndex % 2) != 0)
        {
            yDunIndex--;
            yTilIndex = 1;
        }
        yDunIndex /= 2;

        int32_t tilIndex;
        
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

        int32_t minIndex = level.mTil[dunIndex][tilIndex];
        
        return MinPillar(level.mMin[minIndex], level.mSol.passable(minIndex), minIndex);
    }

    Misc::Helper2D<const Level, const MinPillar> Level::operator[] (int32_t x) const
    {
        return Misc::Helper2D<const Level, const MinPillar>(*this, x, get);
    }
            
    void Level::activate(int32_t x, int32_t y)
    {
        int32_t xDunIndex = x;
        if((xDunIndex % 2) != 0)
            xDunIndex--;
        xDunIndex /= 2;

        int32_t yDunIndex = y;
        if((yDunIndex % 2) != 0)
            yDunIndex--;
        yDunIndex /= 2;

        int32_t index = mDun[xDunIndex][yDunIndex]; 
        
        // open doors when clicked on
        if(mDoorMap.find(index) != mDoorMap.end())
            mDun[xDunIndex][yDunIndex] = mDoorMap[index];
    }
    
    int32_t Level::minSize() const
    {
        return mMin.size();
    }

    const MinPillar Level::minPillar(int32_t i) const
    {
        return MinPillar(mMin[i], mSol.passable(i), i);
    }

    int32_t Level::width() const
    {
        return mDun.width()*2;
    }

    int32_t Level::height() const
    {
        return mDun.height()*2;
    }

    const std::pair<int32_t,int32_t>& Level::upStairsPos() const
    {
        return mUpStairs;
    }

    const std::pair<int32_t,int32_t>& Level::downStairsPos() const
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

    int32_t MinPillar::size() const
    {
        return mData.size();
    }

    int16_t MinPillar::operator[] (int32_t index) const
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
