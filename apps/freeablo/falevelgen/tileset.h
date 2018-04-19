#pragma once
#include <map>
#include <settings/settings.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

namespace Random
{
    class Rng;
}

namespace FALevelGen
{
    enum class TileSetEnum : int32_t
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

    class TileSet
    {
    public:
        explicit TileSet(const std::string& path);

        int32_t getRandomTile(Random::Rng& rng, int32_t tile);
        std::map<int32_t, int32_t> getDoorMap();
        int32_t convert(TileSetEnum val);

    private:
        void fillTile(int32_t tile, Settings::Settings& settings, const std::string& str);
        void loadDoorMap(Settings::Settings& settings);

    public:
        int32_t xWall = 0;
        int32_t outsideXWall = 0;
        int32_t yWall = 0;
        int32_t outsideYWall = 0;
        int32_t bottomCorner = 0;
        int32_t outsideBottomCorner = 0;
        int32_t rightCorner = 0;
        int32_t outsideRightCorner = 0;
        int32_t leftCorner = 0;
        int32_t outsideLeftCorner = 0;
        int32_t topCorner = 0;
        int32_t outsideTopCorner = 0;
        int32_t floor = 0;
        int32_t blank = 0;
        int32_t xWallEnd = 0;
        int32_t xWallEndBack = 0;
        int32_t yWallEnd = 0;
        int32_t yWallEndBack = 0;
        int32_t xDoor = 0;
        int32_t yDoor = 0;

        int32_t upStairs1 = 0;
        int32_t upStairs2 = 0;
        int32_t upStairs3 = 0;

        int32_t upStairs4 = 0;
        int32_t upStairs5 = 0;
        int32_t upStairs6 = 0;

        int32_t upStairs7 = 0;
        int32_t upStairs8 = 0;
        int32_t upStairs9 = 0;

        int32_t downStairs1 = 0;
        int32_t downStairs2 = 0;
        int32_t downStairs3 = 0;

        int32_t downStairs4 = 0;
        int32_t downStairs5 = 0;
        int32_t downStairs6 = 0;

        int32_t downStairs7 = 0;
        int32_t downStairs8 = 0;
        int32_t downStairs9 = 0;

        int32_t insideXWall = 0;
        int32_t insideXWallEnd = 0;
        int32_t insideXWallEndBack = 0;
        int32_t insideYWall = 0;
        int32_t insideYWallEnd = 0;
        int32_t insideYWallEndBack = 0;
        int32_t insideLeftCorner = 0;
        int32_t insideRightCorner = 0;
        int32_t insideBottomCorner = 0;
        int32_t insideTopCorner = 0;

        int32_t joinY = 0;
        int32_t joinYRightCorner = 0;
        int32_t joinRightCorner = 0;
        int32_t joinOutXRightCorner = 0;
        int32_t joinOutX = 0;
        int32_t joinOutXTopCorner = 0;
        int32_t joinTopCorner = 0;
        int32_t joinOutYTopCorner = 0;
        int32_t joinOutY = 0;
        int32_t joinOutYLeftCorner = 0;
        int32_t joinLeftCorner = 0;
        int32_t joinXLeftCorner = 0;
        int32_t joinX = 0;
        int32_t joinXBottomCorner = 0;
        int32_t joinBottomCorner = 0;
        int32_t joinYBottomCorner = 0;

    private:
        std::map<int32_t, std::pair<std::vector<std::pair<int32_t, int32_t>>, int32_t>> mAlternatives;
        std::map<int32_t, int32_t> mDoorMap;
    };
}
