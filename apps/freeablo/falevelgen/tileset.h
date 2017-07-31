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

            int32_t xWall;
            int32_t outsideXWall;
            int32_t yWall;
            int32_t outsideYWall;
            int32_t bottomCorner;
            int32_t outsideBottomCorner;
            int32_t rightCorner;
            int32_t outsideRightCorner;
            int32_t leftCorner;
            int32_t outsideLeftCorner;
            int32_t topCorner;
            int32_t outsideTopCorner;
            int32_t floor;
            int32_t blank;
            int32_t xWallEnd;
            int32_t xWallEndBack;
            int32_t yWallEnd;
            int32_t yWallEndBack;
            int32_t xDoor;
            int32_t yDoor;

            int32_t upStairs1;
            int32_t upStairs2;
            int32_t upStairs3;

            int32_t upStairs4;
            int32_t upStairs5;
            int32_t upStairs6;

            int32_t upStairs7;
            int32_t upStairs8;
            int32_t upStairs9;

            int32_t downStairs1;
            int32_t downStairs2;
            int32_t downStairs3;

            int32_t downStairs4;
            int32_t downStairs5;
            int32_t downStairs6;

            int32_t downStairs7;
            int32_t downStairs8;
            int32_t downStairs9;

            int32_t insideXWall;
            int32_t insideXWallEnd;
            int32_t insideXWallEndBack;
            int32_t insideYWall;
            int32_t insideYWallEnd;
            int32_t insideYWallEndBack;
            int32_t insideLeftCorner;
            int32_t insideRightCorner;
            int32_t insideBottomCorner;
            int32_t insideTopCorner;

            int32_t joinY;
            int32_t joinYRightCorner;
            int32_t joinRightCorner;
            int32_t joinOutXRightCorner;
            int32_t joinOutX;
            int32_t joinOutXTopCorner;
            int32_t joinTopCorner;
            int32_t joinOutYTopCorner;
            int32_t joinOutY;
            int32_t joinOutYLeftCorner;
            int32_t joinLeftCorner;
            int32_t joinXLeftCorner;
            int32_t joinX;
            int32_t joinXBottomCorner;
            int32_t joinBottomCorner;
            int32_t joinYBottomCorner;

            int32_t getRandomTile(int32_t tile);
            std::map<int32_t, int32_t> getDoorMap();

            int32_t convert(TileSetEnum::TileSetEnum val);
        
        private:
            std::map<int32_t, std::pair<std::vector<std::pair<int32_t, int32_t> >, int32_t> > mAlternatives;

            std::map<int32_t, int32_t> mDoorMap;

            void fillTile(int32_t tile, Settings::Settings & settings, const std::string& str);

            void loadDoorMap(Settings::Settings & settings);
    };

}

#endif
