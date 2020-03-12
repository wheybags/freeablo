#include "level.h"
#include <iostream>
#include <serial/loader.h>

namespace Level
{
    Level::Level(Dun&& dun,
                 const std::string& tilPath,
                 const std::string& minPath,
                 const std::string& solPath,
                 const std::string& tileSetPath,
                 const std::string& specialCelPath,
                 const std::map<int32_t, int32_t>& specialCelMap,
                 const Misc::Point& downStairs,
                 const Misc::Point& upStairs,
                 std::map<int32_t, int32_t> doorMap,
                 int32_t previous,
                 int32_t next)
        : mTilesetCelPath(tileSetPath), mSpecialCelPath(specialCelPath), mSpecialCelMap(specialCelMap), mTilPath(tilPath), mMinPath(minPath), mSolPath(solPath),
          mDun(std::move(dun)), mTil(mTilPath), mMin(mMinPath), mSol(mSolPath), mDoorMap(doorMap), mUpStairs(upStairs), mDownStairs(downStairs),
          mPrevious(previous), mNext(next)
    {
    }

    Level::Level(Serial::Loader& loader)
        : mTilesetCelPath(loader.load<std::string>()), mSpecialCelPath(loader.load<std::string>()), mTilPath(loader.load<std::string>()),
          mMinPath(loader.load<std::string>()), mSolPath(loader.load<std::string>()), mDun(loader), mTil(mTilPath), mMin(mMinPath), mSol(mSolPath)
    {
        uint32_t specialCelMapSize = loader.load<uint32_t>();
        for (uint32_t i = 0; i < specialCelMapSize; i++)
        {
            int32_t key = loader.load<int32_t>();
            mSpecialCelMap[key] = loader.load<int32_t>();
        }

        uint32_t doorMapSize = loader.load<uint32_t>();
        for (uint32_t i = 0; i < doorMapSize; i++)
        {
            int32_t key = loader.load<int32_t>();
            mDoorMap[key] = loader.load<int32_t>();
        }

        int32_t first, second;

        first = loader.load<int32_t>();
        second = loader.load<int32_t>();
        mUpStairs = {first, second};

        first = loader.load<int32_t>();
        second = loader.load<int32_t>();
        mDownStairs = {first, second};

        mPrevious = loader.load<int32_t>();
        mNext = loader.load<int32_t>();
    }

    void Level::save(Serial::Saver& saver) const
    {
        Serial::ScopedCategorySaver cat("Level", saver);

        saver.save(mTilesetCelPath);
        saver.save(mSpecialCelPath);
        saver.save(mTilPath);
        saver.save(mMinPath);
        saver.save(mSolPath);
        mDun.save(saver);

        uint32_t specialCelMapSize = mSpecialCelMap.size();
        saver.save(specialCelMapSize);
        for (const auto& entry : mSpecialCelMap)
        {
            saver.save(entry.first);
            saver.save(entry.second);
        }

        uint32_t doorMapSize = mDoorMap.size();
        saver.save(doorMapSize);

        for (const auto& entry : mDoorMap)
        {
            saver.save(entry.first);
            saver.save(entry.second);
        }

        saver.save(mUpStairs.x);
        saver.save(mUpStairs.y);

        saver.save(mDownStairs.x);
        saver.save(mDownStairs.y);

        saver.save(mPrevious);
        saver.save(mNext);
    }

    std::vector<int16_t> Level::mEmpty(16);

    bool Level::isStairs(int32_t x, int32_t y) const
    {
        if (mDownStairs.x == x && mDownStairs.y == y)
            return true;
        if (mUpStairs.x == x && mUpStairs.y == y)
            return true;

        return false;
    }

    Level::InternalLocationData Level::getInternalLocationData(const Misc::Point& point) const
    {
        int32_t xDunIndex = point.x;
        int32_t xTilIndex = 0;
        if ((xDunIndex % 2) != 0)
        {
            xDunIndex--;
            xTilIndex = 1;
        }
        xDunIndex /= 2;

        int32_t yDunIndex = point.y;
        int32_t yTilIndex = 0;
        if ((yDunIndex % 2) != 0)
        {
            yDunIndex--;
            yTilIndex = 1;
        }
        yDunIndex /= 2;

        int32_t tilIndex;

        if (xTilIndex)
        {
            if (yTilIndex)
                tilIndex = 3; // bottom
            else
                tilIndex = 1; // left
        }
        else
        {
            if (yTilIndex)
                tilIndex = 2; // right
            else
                tilIndex = 0; // top
        }

        return {xDunIndex, yDunIndex, tilIndex};
    }

    MinPillar Level::get(const Misc::Point& point) const
    {
        InternalLocationData locationData = getInternalLocationData(point);

        int32_t dunIndex = mDun.get(locationData.xDunIndex, locationData.yDunIndex) - 1;

        if (dunIndex == -1)
            return MinPillar(Level::mEmpty, 0, -1);

        int32_t minIndex = mTil[dunIndex][locationData.tilIndex];

        return MinPillar(mMin[minIndex], mSol.passable(minIndex), minIndex);
    }

    bool Level::isDoor(const Misc::Point& point) const
    {
        InternalLocationData locationData = getInternalLocationData(point);

        int32_t dunIndex = mDun.get(locationData.xDunIndex, locationData.yDunIndex);

        // Ensure point is within the bounds of the dungeon.
        if (mDun.pointIsValid(locationData.xDunIndex, locationData.yDunIndex))
        {
            // open doors when clicked on
            if (mDoorMap.find(dunIndex) != mDoorMap.end())
            {
                bool passableNow = mSol.passable(mTil[dunIndex - 1][locationData.tilIndex]);
                bool passableWhenToggled = mSol.passable(mTil[mDoorMap.at(dunIndex) - 1][locationData.tilIndex]);

                // Only mark the tile(s) that actually change as a door tile
                return passableNow != passableWhenToggled;
            }
        }

        return false;
    }

    bool Level::activateDoor(const Misc::Point& point)
    {
        int32_t xDunIndex = point.x;
        if ((xDunIndex % 2) != 0)
            xDunIndex--;
        xDunIndex /= 2;

        int32_t yDunIndex = point.y;
        if ((yDunIndex % 2) != 0)
            yDunIndex--;
        yDunIndex /= 2;

        // Ensure point is within the bounds of the dungeon.
        if (mDun.pointIsValid(xDunIndex, yDunIndex))
        {
            int32_t index = mDun.get(xDunIndex, yDunIndex);

            // open doors when clicked on
            if (mDoorMap.find(index) != mDoorMap.end())
            {
                mDun.get(xDunIndex, yDunIndex) = mDoorMap[index];
                return true;
            }
        }

        return false;
    }

    int32_t Level::minSize() const { return mMin.size(); }

    const MinPillar Level::minPillar(int32_t i) const { return MinPillar(mMin[i], mSol.passable(i), i); }

    int32_t Level::width() const { return mDun.width() * 2; }

    int32_t Level::height() const { return mDun.height() * 2; }

    const Misc::Point& Level::upStairsPos() const { return mUpStairs; }

    const Misc::Point& Level::downStairsPos() const { return mDownStairs; }

    const std::string& Level::getTileSetPath() const { return mTilesetCelPath; }

    const std::string& Level::getSpecialCelPath() const { return mSpecialCelPath; }

    const std::map<int32_t, int32_t>& Level::getSpecialCelMap() const { return mSpecialCelMap; }

    const std::string& Level::getMinPath() const { return mMinPath; }

    MinPillar::MinPillar(const std::vector<int16_t>& data, bool passable, int32_t index) : mData(data), mPassable(passable), mIndex(index) {}

    int32_t MinPillar::size() const { return mData.size(); }

    int16_t MinPillar::operator[](int32_t index) const { return mData[index]; }

    bool MinPillar::passable() const { return mPassable; }

    int32_t MinPillar::index() const { return mIndex; }
}
