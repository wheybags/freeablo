#include "tileset.h"
#include <faio/faio.h>
#include <misc/assert.h>
#include <misc/misc.h>
#include <misc/stringops.h>
#include <random/random.h>
#include <settings/settings.h>
#include <tinyxml2.h>

namespace FALevelGen
{
    StairsData::StairsData(const tinyxml2::XMLDocument& tmxXml) : tiles(tmxXml), triggerMask(tiles.width() * 2, tiles.height() * 2)
    {
        auto tmxObjectPosToTile = [](const tinyxml2::XMLElement* tmxObject) {
            FixedPoint tmxX(tmxObject->Attribute("x"));
            FixedPoint tmxY(tmxObject->Attribute("y"));

            FixedPoint x = ((tmxX / 64) * 2);
            FixedPoint y = ((tmxY / 64) * 2);

            return Vec2Fix(x, y);
        };

        const tinyxml2::XMLElement* mapElement = tmxXml.FirstChildElement("map");
        const tinyxml2::XMLElement* maskObjectLayer = getFirstChildWithTypeAndAttribute(mapElement, "objectgroup", "name", "triggerMask");

        const tinyxml2::XMLElement* current = maskObjectLayer->FirstChildElement();
        while (current)
        {
            Vec2i point = Vec2i(tmxObjectPosToTile(current));
            triggerMask.get(point.x, point.y) = true;

            current = current->NextSiblingElement();
        }

        const tinyxml2::XMLElement* spawnPointsLayer = getFirstChildWithTypeAndAttribute(mapElement, "objectgroup", "name", "spawnPoints");

        const tinyxml2::XMLElement* enterNode = getFirstChildWithTypeAndAttribute(spawnPointsLayer, "object", "name", "enter");
        spawnOffset = Vec2i(tmxObjectPosToTile(enterNode));

        const tinyxml2::XMLElement* exitNode = getFirstChildWithTypeAndAttribute(spawnPointsLayer, "object", "name", "exit");
        exitOffset = Vec2i(tmxObjectPosToTile(exitNode));
    }

    TileSet::TileSet(const std::string& _path)
    {
        filesystem::path path(_path);

        Settings::Settings settings;
        settings.loadFromFile(path.str());

        filesystem::path basePath = path.parent_path();
        std::string baseName = Misc::StringUtils::getFileNameNoExtension(path.filename());

        filesystem::path upStairsPath = basePath / (baseName + "_stairs_up.tmx");

        if (upStairsPath.exists())
        {
            tinyxml2::XMLDocument upStairsDoc;
            upStairsDoc.LoadFile(upStairsPath.str().c_str());
            upStairsData = StairsData(upStairsDoc);
            upStairsData.onWall = settings.get<bool>("Basic", "upStairsOnWall");
        }
        else
        {
            upStairsData.onWall = settings.get<bool>("Basic", "upStairsOnWall");
            // upStairsData.spawnOffset = {settings.get<int32_t>("Basic", "upStairsXOffset"), settings.get<int32_t>("Basic", "upStairsYOffset")};

            upStairsData.tiles = Level::Dun(3, 3);
            upStairsData.triggerMask = {6, 6};
            upStairsData.tiles.get(0, 0) = settings.get<int32_t>("Basic", "upStairs1");
            upStairsData.tiles.get(1, 0) = settings.get<int32_t>("Basic", "upStairs2");
            upStairsData.tiles.get(2, 0) = settings.get<int32_t>("Basic", "upStairs3");

            upStairsData.tiles.get(0, 1) = settings.get<int32_t>("Basic", "upStairs4");
            upStairsData.tiles.get(1, 1) = settings.get<int32_t>("Basic", "upStairs5");
            upStairsData.tiles.get(2, 1) = settings.get<int32_t>("Basic", "upStairs6");

            upStairsData.tiles.get(0, 2) = settings.get<int32_t>("Basic", "upStairs7");
            upStairsData.tiles.get(1, 2) = settings.get<int32_t>("Basic", "upStairs8");
            upStairsData.tiles.get(2, 2) = settings.get<int32_t>("Basic", "upStairs9");
        }

        // downStairsData = Level::Dun(3, 3);
        downStairsData.tiles = Level::Dun(3, 3);
        downStairsData.triggerMask = {6, 6};
        downStairsData.tiles.get(0, 0) = settings.get<int32_t>("Basic", "downStairs1");
        downStairsData.tiles.get(1, 0) = settings.get<int32_t>("Basic", "downStairs2");
        downStairsData.tiles.get(2, 0) = settings.get<int32_t>("Basic", "downStairs3");

        downStairsData.tiles.get(0, 1) = settings.get<int32_t>("Basic", "downStairs4");
        downStairsData.tiles.get(1, 1) = settings.get<int32_t>("Basic", "downStairs5");
        downStairsData.tiles.get(2, 1) = settings.get<int32_t>("Basic", "downStairs6");

        downStairsData.tiles.get(0, 2) = settings.get<int32_t>("Basic", "downStairs7");
        downStairsData.tiles.get(1, 2) = settings.get<int32_t>("Basic", "downStairs8");
        downStairsData.tiles.get(2, 2) = settings.get<int32_t>("Basic", "downStairs9");

        downStairsData.onWall = settings.get<bool>("Basic", "downStairsOnWall");
        downStairsData.spawnOffset = {settings.get<int32_t>("Basic", "downStairsXOffset"), settings.get<int32_t>("Basic", "downStairsYOffset")};

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

    int32_t TileSet::getRandomTile(Random::Rng& rng, int32_t tile)
    {
        if (mAlternatives.find(tile) == mAlternatives.end())
            return tile;

        std::vector<std::pair<int32_t, int32_t>>& tileVec = mAlternatives[tile].first;
        int32_t normPercent = mAlternatives[tile].second;

        int32_t random = rng.randomInRange(0, 100);

        if (random <= normPercent)
            return tile;

        int32_t max = 0;
        for (const auto& item : tileVec)
            max += item.second;

        random = rng.randomInRange(0, max);

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

    int32_t TileSet::convert(TileSetEnum val)
    {
        switch (val)
        {
            case TileSetEnum::xWall:
                return xWall;
            case TileSetEnum::outsideXWall:
                return outsideXWall;
            case TileSetEnum::yWall:
                return yWall;
            case TileSetEnum::outsideYWall:
                return outsideYWall;
            case TileSetEnum::bottomCorner:
                return bottomCorner;
            case TileSetEnum::outsideBottomCorner:
                return outsideBottomCorner;
            case TileSetEnum::rightCorner:
                return rightCorner;
            case TileSetEnum::outsideRightCorner:
                return outsideRightCorner;
            case TileSetEnum::leftCorner:
                return leftCorner;
            case TileSetEnum::outsideLeftCorner:
                return outsideLeftCorner;
            case TileSetEnum::topCorner:
                return topCorner;
            case TileSetEnum::outsideTopCorner:
                return outsideTopCorner;
            case TileSetEnum::floor:
                return floor;
            case TileSetEnum::blank:
                return blank;
            case TileSetEnum::xDoor:
                return xDoor;
            case TileSetEnum::yDoor:
                return yDoor;
            case TileSetEnum::insideXWall:
                return insideXWall;
            case TileSetEnum::insideXWallEnd:
                return insideXWallEnd;
            case TileSetEnum::insideXWallEndBack:
                return insideXWallEndBack;
            case TileSetEnum::insideYWall:
                return insideYWall;
            case TileSetEnum::insideYWallEnd:
                return insideYWallEnd;
            case TileSetEnum::insideYWallEndBack:
                return insideYWallEndBack;
            case TileSetEnum::insideLeftCorner:
                return insideLeftCorner;
            case TileSetEnum::insideRightCorner:
                return insideRightCorner;
            case TileSetEnum::insideBottomCorner:
                return insideBottomCorner;
            case TileSetEnum::insideTopCorner:
                return insideTopCorner;

            case TileSetEnum::joinY:
                return joinY;
            case TileSetEnum::joinYRightCorner:
                return joinYRightCorner;
            case TileSetEnum::joinRightCorner:
                return joinRightCorner;
            case TileSetEnum::joinOutXRightCorner:
                return joinOutXRightCorner;
            case TileSetEnum::joinOutX:
                return joinOutX;
            case TileSetEnum::joinOutXTopCorner:
                return joinOutXTopCorner;
            case TileSetEnum::joinTopCorner:
                return joinTopCorner;
            case TileSetEnum::joinOutYTopCorner:
                return joinOutYTopCorner;
            case TileSetEnum::joinOutY:
                return joinOutY;
            case TileSetEnum::joinOutYLeftCorner:
                return joinOutYLeftCorner;
            case TileSetEnum::joinLeftCorner:
                return joinLeftCorner;
            case TileSetEnum::joinXLeftCorner:
                return joinXLeftCorner;
            case TileSetEnum::joinX:
                return joinX;
            case TileSetEnum::joinXBottomCorner:
                return joinXBottomCorner;
            case TileSetEnum::joinBottomCorner:
                return joinBottomCorner;
            case TileSetEnum::joinYBottomCorner:
                return joinYBottomCorner;

            // these two just used internally in levelgen.cpp, not loaded from a file like the rest
            case TileSetEnum::upStairs:
                break;
            case TileSetEnum::downStairs:
                break;
        }

        release_assert(false && "tried to convert invalid tile");
        return (int32_t)val;
    }
}
