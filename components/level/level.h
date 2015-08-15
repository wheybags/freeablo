#ifndef LEVEL_H
#define LEVEL_H

#include "min.h"
#include "tileset.h"
#include "dun.h"
#include "sol.h"
#include "baseitemmanager.h"
#include <utility>
#include <map>

namespace Level
{
    struct Monster
    {
        std::string name;
        size_t xPos;
        size_t yPos;
    };

    class Level;

    class MinPillar
    {
        public:
            size_t size() const;
            int16_t operator[] (size_t index) const;
            bool passable() const;
            int32_t index() const;
        
        private:
            MinPillar(const std::vector<int16_t>& data, bool passable, int32_t index);
            const std::vector<int16_t>& mData;
            
            bool mPassable;
            int32_t mIndex;

            friend class Level;
            friend const MinPillar get(size_t x, size_t y, const Level& level);
    };

    class Level
    {
        public:
            Level(const std::string& dunPath, const std::string& tilPath, const std::string& minPath,
                const std::string& solPath, const std::string& tileSetPath, const std::pair<size_t,size_t>& downStairs,
                const std::pair<size_t,size_t>& upStairs, BaseItemManager *itemManager);
            Level(const Dun& dun, const std::string& tilPath, const std::string& minPath, 
                const std::string& solPath, const std::string& tileSetPath, const std::pair<size_t,size_t>& downStairs,
                const std::pair<size_t,size_t>& upStairs, std::map<size_t, size_t> doorMap, BaseItemManager* itemManager);

            Misc::Helper2D<const Level, const MinPillar> operator[] (size_t x) const;

            void activate(size_t x, size_t y);

            size_t minSize() const;
            const MinPillar minPillar(int32_t i) const;

            size_t width() const;
            size_t height() const;

            const std::pair<size_t,size_t>& upStairsPos() const;
            const std::pair<size_t,size_t>& downStairsPos() const;

            const std::string& getTileSetPath() const;
            const std::string& getMinPath() const;

            const std::vector<Monster>& getMonsters() const;
            std::vector<Monster>& getMonsters();

            bool isStairs(size_t, size_t) const;

        private:
            Dun mDun;
            TileSet mTil;
            Min mMin;
            Sol mSol;
            std::string mTileSetPath; ///< path to cel file for level
            std::string mMinPath; ///< path to min file for level

            std::map<size_t, size_t> mDoorMap; ///< Map from closed door indices to open door indices + vice-versa

            std::pair<size_t,size_t> mUpStairs;
            std::pair<size_t,size_t> mDownStairs;

            std::vector<Monster> mMonsters;
            BaseItemManager * mItemManager;
            static std::vector<int16_t> mEmpty;
//            FAWorld::Level::ItemManager mLevel::Items;
            friend const MinPillar get(size_t x, size_t y, const Level& level);
    };
}

#endif
