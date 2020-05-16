#pragma once
#include "baseitemmanager.h"
#include "dun.h"
#include "min.h"
#include "sol.h"
#include "tileset.h"
#include <map>
#include <misc/misc.h>
#include <utility>

namespace Serial
{
    class Loader;
    class Saver;
}

namespace Level
{
    class Level;

    class MinPillar
    {
    public:
        int32_t size() const;
        int16_t operator[](int32_t index) const;
        bool passable() const;
        int32_t index() const;

    private:
        MinPillar(const std::vector<int16_t>& data, bool passable, int32_t index);
        const std::vector<int16_t>& mData;

        bool mPassable;
        int32_t mIndex;

        friend class Level;
    };

    struct LevelTransitionArea
    {
        LevelTransitionArea() = default;
        LevelTransitionArea(LevelTransitionArea&&) = default;
        LevelTransitionArea(const LevelTransitionArea& other);
        LevelTransitionArea(int32_t targetLevelIndex, Vec2i offset, IntRange dimensions, Vec2i playerSpawnOffset, Vec2i exitOffset);

        void save(Serial::Saver& saver) const;
        void load(Serial::Loader& loader);

        bool pointIsInside(Vec2i point) const;

        int32_t targetLevelIndex = -1;
        Vec2i offset = Vec2i::invalid();
        IntRange dimensions;
        Vec2i playerSpawnOffset; // relative to overall offset
        Vec2i exitOffset;
        Misc::Array2D<uint8_t> triggerMask; // true for tiles which trigger level change, false otherwise
    };

    class Level
    {
    public:
        Level(Dun&& dun,
              int32_t tilesetId,
              const std::string& tilPath,
              const std::string& minPath,
              const std::string& solPath,
              const std::string& tileSetPath,
              const std::string& specialCelPath,
              const std::map<int32_t, int32_t>& specialCelMap,
              const LevelTransitionArea& upStairs,
              const LevelTransitionArea& downStairs,
              std::map<int32_t, int32_t> doorMap);

        explicit Level(Serial::Loader& loader);
        Level() = default;

        void save(Serial::Saver& saver) const;

        bool isDoor(const Misc::Point& point) const;
        bool activateDoor(const Misc::Point& point); /// @return If the door was activated

        MinPillar get(const Misc::Point& point) const;

        int32_t width() const;
        int32_t height() const;

        const LevelTransitionArea& upStairsArea() const { return mUpStairs; };
        const LevelTransitionArea& downStairsArea() const { return mDownStairs; }

        int32_t getTilesetId() const { return mTilesetId; }
        const std::map<int32_t, int32_t>& getSpecialCelMap() const { return mSpecialCelMap; }

        int32_t getNextLevel() const { return mDownStairs.targetLevelIndex; }
        int32_t getPreviousLevel() const { return mUpStairs.targetLevelIndex; }

    private:
        struct InternalLocationData
        {
            int32_t xDunIndex = 0;
            int32_t yDunIndex = 0;
            int32_t tilIndex = 0;
        };

        InternalLocationData getInternalLocationData(const Misc::Point& point) const;

    private:
        int32_t mTilesetId = 0;
        std::string mTilesetCelPath;               ///< path to cel file for level
        std::string mSpecialCelPath;               ///< path to special cel file for level (mostly used for arches / open doors).
        std::map<int32_t, int32_t> mSpecialCelMap; ///< Map from tileset frame number to special cel frame number
        std::string mTilPath;                      ///< path to til file for level
        std::string mMinPath;                      ///< path to min file for level
        std::string mSolPath;                      ///< path to sol file for this level

        Dun mDun;
        TileSet mTil;
        Min mMin;
        Sol mSol;

        std::map<int32_t, int32_t> mDoorMap; ///< Map from closed door indices to open door indices + vice-versa

        LevelTransitionArea mUpStairs;
        LevelTransitionArea mDownStairs;

        static std::vector<int16_t> mEmpty;
    };
}
