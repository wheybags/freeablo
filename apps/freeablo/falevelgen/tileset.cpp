#include "tileset.h"

#include <algorithm>
#include <sstream>

#include <faio/faio.h>
#include <misc/assert.h>
#include <settings/settings.h>

#include "random.h"

namespace FALevelGen
{
    TileSet::TileSet(const std::string& path)
    {
        Settings::Settings settings;
        settings.loadFromFile(path);

        xWall = settings.get<int32_t>("Basic", "xWall");
        fillTile(xWall, settings, "XWall");

        outsideXWall = settings.get<int32_t>("Basic", "outsideXWall");
        fillTile(outsideXWall, settings, "outsideXWall");

        yWall = settings.get<int32_t>("Basic", "yWall");
        fillTile(yWall, settings, "YWall");

        outsideYWall = settings.get<int32_t>("Basic", "outsideYWall");
        fillTile(outsideYWall, settings, "OutsideYWall");

        bottomCorner = settings.get<int32_t>("Basic", "bottomCorner");
        fillTile(bottomCorner, settings, "BottomCorner");

        outsideBottomCorner = settings.get<int32_t>("Basic", "outsideBottomCorner");
        fillTile(outsideBottomCorner, settings, "OutsideBottomCorner");

        rightCorner = settings.get<int32_t>("Basic", "rightCorner");
        fillTile(rightCorner, settings, "RightCorner");

        outsideRightCorner = settings.get<int32_t>("Basic", "outsideRightCorner");
        fillTile(outsideRightCorner, settings, "OutsideRightCorner");

        leftCorner = settings.get<int32_t>("Basic", "leftCorner");
        fillTile(leftCorner, settings, "LeftCorner");

        outsideLeftCorner = settings.get<int32_t>("Basic", "outsideLeftCorner");
        fillTile(outsideLeftCorner, settings, "OutsideLeftCorner");

        topCorner = settings.get<int32_t>("Basic", "topCorner");
        fillTile(topCorner, settings, "TopCorner");

        outsideTopCorner = settings.get<int32_t>("Basic", "outsideTopCorner");
        fillTile(outsideTopCorner, settings, "OutsideTopCorner");

        floor = settings.get<int32_t>("Basic", "floor");
        fillTile(floor, settings, "Floor");

        blank = settings.get<int32_t>("Basic", "blank");
        fillTile(blank, settings, "Blank");

        xDoor = settings.get<int32_t>("Basic", "xDoor");
        fillTile(xDoor, settings, "XDoor");

        yDoor = settings.get<int32_t>("Basic", "yDoor");
        fillTile(yDoor, settings, "YDoor");

        insideXWall = settings.get<int32_t>("Basic", "insideXWall");
        fillTile(insideXWall, settings, "InsideXWall");
        insideXWallEnd = settings.get<int32_t>("Basic", "insideXWallEnd");
        fillTile(insideXWallEnd, settings, "InsideXWallEnd");
        insideXWallEndBack = settings.get<int32_t>("Basic", "insideXWallEndBack");
        fillTile(insideXWallEndBack, settings, "InsideXWallEndBack");
        insideYWall = settings.get<int32_t>("Basic", "insideYWall");
        fillTile(insideYWall, settings, "InsideYWall");
        insideYWallEnd = settings.get<int32_t>("Basic", "insideYWallEnd");
        fillTile(insideYWallEnd, settings, "InsideYWallEnd");
        insideYWallEndBack = settings.get<int32_t>("Basic", "insideYWallEndBack");
        fillTile(insideYWallEndBack, settings, "InsideYWallEndBack");
        insideLeftCorner = settings.get<int32_t>("Basic", "insideLeftCorner");
        fillTile(insideLeftCorner, settings, "InsideLeftCorner");
        insideRightCorner = settings.get<int32_t>("Basic", "insideRightCorner");
        fillTile(insideRightCorner, settings, "InsideRightCorner");
        insideBottomCorner = settings.get<int32_t>("Basic", "insideBottomCorner");
        fillTile(insideBottomCorner, settings, "InsideBottomCorner");
        insideTopCorner = settings.get<int32_t>("Basic", "insideTopCorner");
        fillTile(insideTopCorner, settings, "InsideTopCorner");

        joinY = settings.get<int32_t>("Basic", "joinY");
        fillTile(joinY, settings, "JoinY");
        joinYRightCorner = settings.get<int32_t>("Basic", "joinYRightCorner");
        fillTile(joinYRightCorner, settings, "JoinYRightCorner");
        joinRightCorner = settings.get<int32_t>("Basic", "joinRightCorner");
        fillTile(joinRightCorner, settings, "JoinRightCorner");
        joinOutXRightCorner = settings.get<int32_t>("Basic", "joinOutXRightCorner");
        fillTile(joinOutXRightCorner, settings, "JoinOutXRightCorner");
        joinOutX = settings.get<int32_t>("Basic", "joinOutX");
        fillTile(joinOutX, settings, "JoinOutX");
        joinOutXTopCorner = settings.get<int32_t>("Basic", "joinOutXTopCorner");
        fillTile(joinOutXTopCorner, settings, "JoinOutXTopCorner");
        joinTopCorner = settings.get<int32_t>("Basic", "joinTopCorner");
        fillTile(joinTopCorner, settings, "JoinTopCorner");
        joinOutYTopCorner = settings.get<int32_t>("Basic", "joinOutYTopCorner");
        fillTile(joinOutYTopCorner, settings, "JoinOutYTopCorner");
        joinOutY = settings.get<int32_t>("Basic", "joinOutY");
        fillTile(joinOutY, settings, "JoinOutY");
        joinOutYLeftCorner = settings.get<int32_t>("Basic", "joinOutYLeftCorner");
        fillTile(joinOutYLeftCorner, settings, "JoinOutYLeftCorner");
        joinLeftCorner = settings.get<int32_t>("Basic", "joinLeftCorner");
        fillTile(joinLeftCorner, settings, "JoinLeftCorner");
        joinXLeftCorner = settings.get<int32_t>("Basic", "joinXLeftCorner");
        fillTile(joinXLeftCorner, settings, "JoinXLeftCorner");
        joinX = settings.get<int32_t>("Basic", "joinX");
        fillTile(joinX, settings, "JoinX");
        joinXBottomCorner = settings.get<int32_t>("Basic", "joinXBottomCorner");
        fillTile(joinXBottomCorner, settings, "JoinXBottomCorner");
        joinBottomCorner = settings.get<int32_t>("Basic", "joinBottomCorner");
        fillTile(joinBottomCorner, settings, "JoinBottomCorner");
        joinYBottomCorner = settings.get<int32_t>("Basic", "joinYBottomCorner");
        fillTile(joinYBottomCorner, settings, "JoinYBottomCorner");

        upStairs1 = settings.get<int32_t>("Basic", "upStairs1");
        upStairs2 = settings.get<int32_t>("Basic", "upStairs2");
        upStairs3 = settings.get<int32_t>("Basic", "upStairs3");

        upStairs4 = settings.get<int32_t>("Basic", "upStairs4");
        upStairs5 = settings.get<int32_t>("Basic", "upStairs5");
        upStairs6 = settings.get<int32_t>("Basic", "upStairs6");

        upStairs7 = settings.get<int32_t>("Basic", "upStairs7");
        upStairs8 = settings.get<int32_t>("Basic", "upStairs8");
        upStairs9 = settings.get<int32_t>("Basic", "upStairs9");

        downStairs1 = settings.get<int32_t>("Basic", "downStairs1");
        downStairs2 = settings.get<int32_t>("Basic", "downStairs2");
        downStairs3 = settings.get<int32_t>("Basic", "downStairs3");

        downStairs4 = settings.get<int32_t>("Basic", "downStairs4");
        downStairs5 = settings.get<int32_t>("Basic", "downStairs5");
        downStairs6 = settings.get<int32_t>("Basic", "downStairs6");

        downStairs7 = settings.get<int32_t>("Basic", "downStairs7");
        downStairs8 = settings.get<int32_t>("Basic", "downStairs8");
        downStairs9 = settings.get<int32_t>("Basic", "downStairs9");

        loadDoorMap(settings);
    }

    void TileSet::fillTile(int32_t tile, Settings::Settings& settings, const std::string& section)
    {
        std::vector<std::pair<int32_t, int32_t>> tileVec;
        int32_t normPercent = 100;

        if (settings.isSectionExists(section))
        {
            Settings::Container properties = settings.getPropertiesInSection(section);
            int32_t size = properties.size();

            for (int32_t i = 0; i < size; i++)
            {
                if (properties[i] != "normal")
                {
                    std::stringstream buffer(properties[i]);
                    int32_t propertyAsSizeT;
                    buffer >> propertyAsSizeT;

                    int32_t value = settings.get<int32_t>(section, properties[i]);
                    tileVec.push_back(std::pair<int32_t, int32_t>(propertyAsSizeT, value));
                }
            }

            normPercent = settings.get<int32_t>(section, "normal");
        }

        mAlternatives[tile] = std::pair<std::vector<std::pair<int32_t, int32_t>>, int32_t>(tileVec, normPercent);
    }

    int32_t TileSet::getRandomTile(int32_t tile)
    {
        if (mAlternatives.find(tile) == mAlternatives.end())
            return tile;

        std::vector<std::pair<int32_t, int32_t>>& tileVec = mAlternatives[tile].first;
        int32_t normPercent = mAlternatives[tile].second;

        int32_t random = randomInRange(0, 100);

        if (random <= normPercent)
            return tile;

        int32_t max = 0;
        for (int32_t i = 0; i < (int32_t)tileVec.size(); i++)
            max += tileVec[i].second;

        random = randomInRange(0, max);

        int32_t i = 0;
        for (; i < (int32_t)tileVec.size() && random > tileVec[i].second; i++)
            random -= tileVec[i].second;

        return tileVec[i].first;
    }

    std::map<int32_t, int32_t> TileSet::getDoorMap() { return mDoorMap; }

    void TileSet::loadDoorMap(Settings::Settings& settings)
    {
        Settings::Container properties = settings.getPropertiesInSection("DoorMap");

        for (Settings::Container::const_iterator it = properties.begin(); it != properties.end(); ++it)
        {
            std::stringstream buffer(*it);
            int32_t propertyAsSizeT;
            buffer >> propertyAsSizeT;

            int32_t value = settings.get<int32_t>("DoorMap", *it);

            mDoorMap[propertyAsSizeT] = value;
            mDoorMap[value] = propertyAsSizeT;
        }
    }

    int32_t TileSet::convert(int32_t val)
    {
        switch (val)
        {
            case (int32_t)TileSetEnum::xWall:
                return xWall;
            case (int32_t)TileSetEnum::outsideXWall:
                return outsideXWall;
            case (int32_t)TileSetEnum::yWall:
                return yWall;
            case (int32_t)TileSetEnum::outsideYWall:
                return outsideYWall;
            case (int32_t)TileSetEnum::bottomCorner:
                return bottomCorner;
            case (int32_t)TileSetEnum::outsideBottomCorner:
                return outsideBottomCorner;
            case (int32_t)TileSetEnum::rightCorner:
                return rightCorner;
            case (int32_t)TileSetEnum::outsideRightCorner:
                return outsideRightCorner;
            case (int32_t)TileSetEnum::leftCorner:
                return leftCorner;
            case (int32_t)TileSetEnum::outsideLeftCorner:
                return outsideLeftCorner;
            case (int32_t)TileSetEnum::topCorner:
                return topCorner;
            case (int32_t)TileSetEnum::outsideTopCorner:
                return outsideTopCorner;
            case (int32_t)TileSetEnum::floor:
                return floor;
            case (int32_t)TileSetEnum::blank:
                return blank;
            case (int32_t)TileSetEnum::xDoor:
                return xDoor;
            case (int32_t)TileSetEnum::yDoor:
                return yDoor;

            case (int32_t)TileSetEnum::upStairs1:
                return upStairs1;
            case (int32_t)TileSetEnum::upStairs2:
                return upStairs2;
            case (int32_t)TileSetEnum::upStairs3:
                return upStairs3;

            case (int32_t)TileSetEnum::upStairs4:
                return upStairs4;
            case (int32_t)TileSetEnum::upStairs5:
                return upStairs5;
            case (int32_t)TileSetEnum::upStairs6:
                return upStairs6;

            case (int32_t)TileSetEnum::upStairs7:
                return upStairs7;
            case (int32_t)TileSetEnum::upStairs8:
                return upStairs8;
            case (int32_t)TileSetEnum::upStairs9:
                return upStairs9;

            case (int32_t)TileSetEnum::downStairs1:
                return downStairs1;
            case (int32_t)TileSetEnum::downStairs2:
                return downStairs2;
            case (int32_t)TileSetEnum::downStairs3:
                return downStairs3;

            case (int32_t)TileSetEnum::downStairs4:
                return downStairs4;
            case (int32_t)TileSetEnum::downStairs5:
                return downStairs5;
            case (int32_t)TileSetEnum::downStairs6:
                return downStairs6;

            case (int32_t)TileSetEnum::downStairs7:
                return downStairs7;
            case (int32_t)TileSetEnum::downStairs8:
                return downStairs8;
            case (int32_t)TileSetEnum::downStairs9:
                return downStairs9;

            case (int32_t)TileSetEnum::insideXWall:
                return insideXWall;
            case (int32_t)TileSetEnum::insideXWallEnd:
                return insideXWallEnd;
            case (int32_t)TileSetEnum::insideXWallEndBack:
                return insideXWallEndBack;
            case (int32_t)TileSetEnum::insideYWall:
                return insideYWall;
            case (int32_t)TileSetEnum::insideYWallEnd:
                return insideYWallEnd;
            case (int32_t)TileSetEnum::insideYWallEndBack:
                return insideYWallEndBack;
            case (int32_t)TileSetEnum::insideLeftCorner:
                return insideLeftCorner;
            case (int32_t)TileSetEnum::insideRightCorner:
                return insideRightCorner;
            case (int32_t)TileSetEnum::insideBottomCorner:
                return insideBottomCorner;
            case (int32_t)TileSetEnum::insideTopCorner:
                return insideTopCorner;

            case (int32_t)TileSetEnum::joinY:
                return joinY;
            case (int32_t)TileSetEnum::joinYRightCorner:
                return joinYRightCorner;
            case (int32_t)TileSetEnum::joinRightCorner:
                return joinRightCorner;
            case (int32_t)TileSetEnum::joinOutXRightCorner:
                return joinOutXRightCorner;
            case (int32_t)TileSetEnum::joinOutX:
                return joinOutX;
            case (int32_t)TileSetEnum::joinOutXTopCorner:
                return joinOutXTopCorner;
            case (int32_t)TileSetEnum::joinTopCorner:
                return joinTopCorner;
            case (int32_t)TileSetEnum::joinOutYTopCorner:
                return joinOutYTopCorner;
            case (int32_t)TileSetEnum::joinOutY:
                return joinOutY;
            case (int32_t)TileSetEnum::joinOutYLeftCorner:
                return joinOutYLeftCorner;
            case (int32_t)TileSetEnum::joinLeftCorner:
                return joinLeftCorner;
            case (int32_t)TileSetEnum::joinXLeftCorner:
                return joinXLeftCorner;
            case (int32_t)TileSetEnum::joinX:
                return joinX;
            case (int32_t)TileSetEnum::joinXBottomCorner:
                return joinXBottomCorner;
            case (int32_t)TileSetEnum::joinBottomCorner:
                return joinBottomCorner;
            case (int32_t)TileSetEnum::joinYBottomCorner:
                return joinYBottomCorner;

            // these two just used internally in levelgen.cpp, not loaded from a file like the rest
            case (int32_t)TileSetEnum::upStairs:
                break;
            case (int32_t)TileSetEnum::downStairs:
                break;
        }

        release_assert(false && "tried to convert invalid tile");
        return val;
    }
}
