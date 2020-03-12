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
        friend const MinPillar get(int32_t x, int32_t y, const Level& level);
    };

    class Level
    {
    public:
        Level(Dun&& dun,
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
              int32_t next);

        Level(Serial::Loader& loader);

        Level() {}

        void save(Serial::Saver& saver) const;

        bool isDoor(const Misc::Point& point) const;
        bool activateDoor(const Misc::Point& point); /// @return If the door was activated

        int32_t minSize() const;
        const MinPillar minPillar(int32_t i) const;

        MinPillar get(const Misc::Point& point) const;

        int32_t width() const;
        int32_t height() const;

        const Misc::Point& upStairsPos() const;
        const Misc::Point& downStairsPos() const;

        const std::string& getTileSetPath() const;
        const std::string& getSpecialCelPath() const;
        const std::map<int32_t, int32_t>& getSpecialCelMap() const;
        const std::string& getMinPath() const;

        bool isStairs(int32_t, int32_t) const;

        int32_t getNextLevel() const { return mNext; }

        int32_t getPreviousLevel() const { return mPrevious; }

    private:
        struct InternalLocationData
        {
            int32_t xDunIndex = 0;
            int32_t yDunIndex = 0;
            int32_t tilIndex = 0;
        };

        InternalLocationData getInternalLocationData(const Misc::Point& point) const;

    private:
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

        Misc::Point mUpStairs;
        Misc::Point mDownStairs;

        static std::vector<int16_t> mEmpty;
        friend const MinPillar get(int32_t x, int32_t y, const Level& level);

        int32_t mPrevious; ///< index of previous level
        int32_t mNext;     ///< index of next level
    };
}
