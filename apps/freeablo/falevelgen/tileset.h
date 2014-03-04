#ifndef FA_TILESET_H
#define FA_TILESET_H

#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include <boost/property_tree/ptree.hpp>

namespace FALevelGen
{
    class TileSet
    {
        public:
            TileSet(const std::string& path);

            size_t xWall;
            size_t outsideXWall;
            size_t yWall;
            size_t outsideYWall;
            size_t bottomCorner;
            size_t outsideBottomCorner;
            size_t rightCorner;
            size_t outsideRightCorner;
            size_t leftCorner;
            size_t outsideLeftCorner;
            size_t topCorner;
            size_t outsideTopCorner;
            size_t floor;
            size_t blank;
            size_t xWallEnd;
            size_t yWallEnd;
            size_t xDoor;
            size_t yDoor;

            size_t getRandomTile(size_t tile);
        
        private:
            std::map<size_t, std::pair<std::vector<std::pair<size_t, size_t> >, size_t> > mAlternatives;

            void fillTile(size_t tile, boost::property_tree::ptree& pt, const std::string& str);

    };

}

#endif
