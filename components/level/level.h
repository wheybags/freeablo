#ifndef LEVEL_H
#define LEVEL_H

#include "min.h"
#include "tileset.h"
#include "dun.h"
#include "sol.h"
#include "baseitemmanager.h"
#include <utility>
#include <map>

#include <misc/misc.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

namespace Level
{
    class Level;

    class MinPillar
    {
        public:
            int32_t size() const;
            int16_t operator[] (int32_t index) const;
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
            Level(const Dun& dun, const std::string& tilPath, const std::string& minPath,
                const std::string& solPath, const std::string& tileSetPath, const std::pair<int32_t,int32_t>& downStairs,
                const std::pair<int32_t,int32_t>& upStairs, std::map<int32_t, int32_t> doorMap, int32_t previous, int32_t next);

            Level() {}

            Misc::Helper2D<const Level, const MinPillar> operator[] (int32_t x) const;

            void activate(int32_t x, int32_t y);

            int32_t minSize() const;
            const MinPillar minPillar(int32_t i) const;

            int32_t width() const;
            int32_t height() const;

            const std::pair<int32_t,int32_t>& upStairsPos() const;
            const std::pair<int32_t,int32_t>& downStairsPos() const;

            const std::string& getTileSetPath() const;
            const std::string& getMinPath() const;

            bool isStairs(int32_t, int32_t) const;

            int32_t getNextLevel() const
            {
                return mNext;
            }

            int32_t getPreviousLevel() const
            {
                return mPrevious;
            }

        private:
            Dun mDun;
            TileSet mTil;
            Min mMin;
            Sol mSol;
            std::string mTileSetPath; ///< path to cel file for level
            std::string mTilPath; ///< path to til file for level
            std::string mMinPath; ///< path to min file for level
            std::string mSolPath; ///< path to sol file for this level

            std::map<int32_t, int32_t> mDoorMap; ///< Map from closed door indices to open door indices + vice-versa

            std::pair<int32_t,int32_t> mUpStairs;
            std::pair<int32_t,int32_t> mDownStairs;

            static std::vector<int16_t> mEmpty;
            friend const MinPillar get(int32_t x, int32_t y, const Level& level);

            int32_t mPrevious; ///< index of previous level
            int32_t mNext; ///< index of next level

            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive& ar, const unsigned int version) const
            {
                UNUSED_PARAM(version);

                ar & mDun;

                ar & mTileSetPath & mTilPath & mMinPath & mSolPath;

                ar & mDoorMap;
                ar & mUpStairs & mDownStairs;
                ar & mPrevious & mNext;
            }

            template<class Archive>
            void load(Archive& ar, const unsigned int version)
            {
                UNUSED_PARAM(version);

                ar & mDun;

                ar & mTileSetPath & mTilPath & mMinPath & mSolPath;
                mTil = TileSet(mTilPath); mMin = Min(mMinPath); mSol = Sol(mSolPath);

                ar & mDoorMap;
                ar & mUpStairs & mDownStairs;
                ar & mPrevious & mNext;
            }

            BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}

#endif
