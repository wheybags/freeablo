#ifndef LEVEL_H
#define LEVEL_H

#include "min.h"
#include "tileset.h"
#include "dun.h"
#include "sol.h"

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
        
        private:
            MinPillar(const std::vector<int16_t>& data, bool passable);
            const std::vector<int16_t>& mData;
            
            bool mPassable;

            friend class Level;
            friend const MinPillar get(size_t x, size_t y, const Level& level);
    };

    class Level
    {
        public:
            Level(const std::string& dunPath, const std::string& tilPath, const std::string& minPath, const std::string& solPath);
            Level(const Dun& dun, const std::string& tilPath, const std::string& minPath, const std::string& solPath);

            Misc::Helper2D<const Level, const MinPillar> operator[] (size_t x) const;

            size_t width() const;
            size_t height() const;

            const std::vector<Monster>& getMonsters() const;
            std::vector<Monster>& getMonsters();

        private:
            Dun mDun;
            TileSet mTil;
            Min mMin;
            Sol mSol;

            std::vector<Monster> mMonsters;
            
            static std::vector<int16_t> mEmpty;

            friend const MinPillar get(size_t x, size_t y, const Level& level);
    };
}

#endif
