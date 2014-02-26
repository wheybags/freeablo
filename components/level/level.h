#ifndef LEVEL_H
#define LEVEL_H

#include "min.h"
#include "tileset.h"
#include "dun.h"

namespace Level
{
    class Level
    {
        public:
            Level(const std::string& dunPath, const std::string& tilPath, const std::string& minPath);
            Level(const Dun& dun, const std::string& tilPath, const std::string& minPath);

            Misc::Helper2D<const Level, const MinPillar&> operator[] (size_t x) const;

            size_t width() const;
            size_t height() const;

        private:
            Dun mDun;
            TileSet mTil;
            Min mMin;

            static MinPillar mEmpty;

            friend const MinPillar& get(size_t x, size_t y, const Level& level);
    };
}

#endif
