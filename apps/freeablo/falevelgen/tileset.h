#ifndef FA_TILESET_H
#define FA_TILESET_H

#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <settings/settings.h>

namespace FALevelGen
{

    namespace TileSetEnum
    {
        enum TileSetEnum
        {
            // this block and the block after it are related
            // eg insideYWall MUST be equal to yWall + insideXWall
            xWall,
            yWall,
            leftCorner,
            rightCorner,
            bottomCorner,
            topCorner,

            insideXWall,
            insideYWall,
            insideLeftCorner,
            insideRightCorner,
            insideBottomCorner,
            insideTopCorner,

            insideXWallEnd,
            insideXWallEndBack,
            insideYWallEnd,
            insideYWallEndBack,



            outsideXWall,
            outsideYWall,
            outsideBottomCorner,
            outsideRightCorner,
            outsideLeftCorner,
            outsideTopCorner,
            floor,
            blank,
            xDoor,
            yDoor,


            joinY,
            joinYRightCorner,
            joinRightCorner,
            joinOutXRightCorner,
            joinOutX,
            joinOutXTopCorner,
            joinTopCorner,
            joinOutYTopCorner,
            joinOutY,
            joinOutYLeftCorner,
            joinLeftCorner,
            joinXLeftCorner,
            joinX,
            joinXBottomCorner,
            joinBottomCorner,
            joinYBottomCorner,

            upStairs1,
            upStairs2,
            upStairs3,

            upStairs4,
            upStairs5,
            upStairs6,

            upStairs7,
            upStairs8,
            upStairs9,

            downStairs1,
            downStairs2,
            downStairs3,

            downStairs4,
            downStairs5,
            downStairs6,

            downStairs7,
            downStairs8,
            downStairs9,

            // these two just used internally in levelgen.cpp, not loaded from a file like the rest
            upStairs,
            downStairs
        };
    }

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
            size_t xWallEndBack;
            size_t yWallEnd;
            size_t yWallEndBack;
            size_t xDoor;
            size_t yDoor;

            size_t upStairs1;
            size_t upStairs2;
            size_t upStairs3;

            size_t upStairs4;
            size_t upStairs5;
            size_t upStairs6;

            size_t upStairs7;
            size_t upStairs8;
            size_t upStairs9;

            size_t downStairs1;
            size_t downStairs2;
            size_t downStairs3;

            size_t downStairs4;
            size_t downStairs5;
            size_t downStairs6;

            size_t downStairs7;
            size_t downStairs8;
            size_t downStairs9;

            size_t insideXWall;
            size_t insideXWallEnd;
            size_t insideXWallEndBack;
            size_t insideYWall;
            size_t insideYWallEnd;
            size_t insideYWallEndBack;
            size_t insideLeftCorner;
            size_t insideRightCorner;
            size_t insideBottomCorner;
            size_t insideTopCorner;

            size_t joinY;
            size_t joinYRightCorner;
            size_t joinRightCorner;
            size_t joinOutXRightCorner;
            size_t joinOutX;
            size_t joinOutXTopCorner;
            size_t joinTopCorner;
            size_t joinOutYTopCorner;
            size_t joinOutY;
            size_t joinOutYLeftCorner;
            size_t joinLeftCorner;
            size_t joinXLeftCorner;
            size_t joinX;
            size_t joinXBottomCorner;
            size_t joinBottomCorner;
            size_t joinYBottomCorner;

            size_t getRandomTile(size_t tile);
            std::map<size_t, size_t> getDoorMap();

            size_t convert(TileSetEnum::TileSetEnum val);
        
        private:
            std::map<size_t, std::pair<std::vector<std::pair<size_t, size_t> >, size_t> > mAlternatives;

            std::map<size_t, size_t> mDoorMap;

            void fillTile(size_t tile, Settings::Settings & settings, const std::string& str);

            void loadDoorMap(Settings::Settings & settings);
    };

}

#endif
