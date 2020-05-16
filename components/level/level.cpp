#include "level.h"
#include <iostream>
#include <serial/loader.h>

namespace Level
{
    Level::Level(Dun&& dun,
                 int32_t tilesetId,
                 const std::string& tilPath,
                 const std::string& minPath,
                 const std::string& solPath,
                 const std::string& tileSetPath,
                 const std::string& specialCelPath,
                 const std::map<int32_t, int32_t>& specialCelMap,
                 const LevelTransitionArea& upStairs,
                 const LevelTransitionArea& downStairs,
                 std::map<int32_t, int32_t> doorMap)
        : mTilesetId(tilesetId), mTilesetCelPath(tileSetPath), mSpecialCelPath(specialCelPath), mSpecialCelMap(specialCelMap), mTilPath(tilPath),
          mMinPath(minPath), mSolPath(solPath), mDun(std::move(dun)), mTil(mTilPath), mMin(mMinPath), mSol(mSolPath), mDoorMap(doorMap), mUpStairs(upStairs),
          mDownStairs(downStairs)
    {
    }

    Level::Level(Serial::Loader& loader)
        : mTilesetId(loader.load<int32_t>()), mTilesetCelPath(loader.load<std::string>()), mSpecialCelPath(loader.load<std::string>()),
          mTilPath(loader.load<std::string>()), mMinPath(loader.load<std::string>()), mSolPath(loader.load<std::string>()), mDun(loader), mTil(mTilPath),
          mMin(mMinPath), mSol(mSolPath)
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

        mUpStairs.load(loader);
        mDownStairs.load(loader);
    }

    void Level::save(Serial::Saver& saver) const
    {
        Serial::ScopedCategorySaver cat("Level", saver);

        saver.save(mTilesetId);
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

        mUpStairs.save(saver);
        mDownStairs.save(saver);
    }

    std::vector<int16_t> Level::mEmpty(16);

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

    int32_t Level::width() const { return mDun.width() * 2; }

    int32_t Level::height() const { return mDun.height() * 2; }

    MinPillar::MinPillar(const std::vector<int16_t>& data, bool passable, int32_t index) : mData(data), mPassable(passable), mIndex(index) {}

    int32_t MinPillar::size() const { return mData.size(); }

    int16_t MinPillar::operator[](int32_t index) const { return mData[index]; }

    bool MinPillar::passable() const { return mPassable; }

    int32_t MinPillar::index() const { return mIndex; }

    LevelTransitionArea::LevelTransitionArea(const LevelTransitionArea& other)
    {
        this->targetLevelIndex = other.targetLevelIndex;
        this->offset = other.offset;
        this->dimensions = other.dimensions;
        this->playerSpawnOffset = other.playerSpawnOffset;
        this->exitOffset = other.exitOffset;
        other.triggerMask.memcpyTo(this->triggerMask);
    }

    LevelTransitionArea::LevelTransitionArea(int32_t targetLevelIndex, Vec2i offset, IntRange dimensions, Vec2i playerSpawnOffset, Vec2i exitOffset)
        : targetLevelIndex(targetLevelIndex), offset(offset), dimensions(dimensions), playerSpawnOffset(playerSpawnOffset), exitOffset(exitOffset),
          triggerMask(dimensions.w, dimensions.h)
    {
    }

    void LevelTransitionArea::save(Serial::Saver& saver) const
    {
        saver.save(targetLevelIndex);
        offset.save(saver);
        dimensions.save(saver);
        playerSpawnOffset.save(saver);
        exitOffset.save(saver);

        int32_t size = triggerMask.width() * triggerMask.height();
        saver.save(size);
        for (bool val : triggerMask)
            saver.save(val);
        saver.save(triggerMask.width());
        saver.save(triggerMask.height());
    }

    void LevelTransitionArea::load(Serial::Loader& loader)
    {
        targetLevelIndex = loader.load<int32_t>();
        offset = Vec2i(loader);
        dimensions = IntRange(loader);
        playerSpawnOffset = Vec2i(loader);
        exitOffset = Vec2i(loader);

        int32_t size = loader.load<int32_t>();
        std::vector<uint8_t> tmp;
        tmp.reserve(size);
        for (int32_t i = 0; i < size; i++)
            tmp.push_back(loader.load<bool>());

        int32_t width = loader.load<int32_t>();
        int32_t height = loader.load<int32_t>();

        triggerMask = Misc::Array2D<uint8_t>(width, height, std::move(tmp));
    }

    bool LevelTransitionArea::pointIsInside(Vec2i point) const
    {
        return point.x >= offset.x && point.x <= offset.x + dimensions.w && point.y >= offset.y && point.y <= offset.y + dimensions.h;
    }
}
