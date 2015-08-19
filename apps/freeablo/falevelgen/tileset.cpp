#include "tileset.h"

#include <sstream>
#include <algorithm>

#include <faio/faio.h>
#include <settings/settings.h>

#include "random.h"

namespace FALevelGen
{
    TileSet::TileSet(const std::string& path)
    {
        Settings::Settings settings;
        settings.loadFromFile(path);

        xWall = settings.get<size_t>("Basic","xWall");
        fillTile(xWall, settings, "XWall");
        
        outsideXWall = settings.get<size_t>("Basic","outsideXWall");
        fillTile(outsideXWall, settings, "outsideXWall");

        yWall = settings.get<size_t>("Basic","yWall");
        fillTile(yWall, settings, "YWall");

        outsideYWall = settings.get<size_t>("Basic","outsideYWall");
        fillTile(outsideYWall, settings, "OutsideYWall");

        bottomCorner = settings.get<size_t>("Basic","bottomCorner");
        fillTile(bottomCorner, settings, "BottomCorner");

        outsideBottomCorner = settings.get<size_t>("Basic","outsideBottomCorner");
        fillTile(outsideBottomCorner, settings, "OutsideBottomCorner");

        rightCorner = settings.get<size_t>("Basic","rightCorner");
        fillTile(rightCorner, settings, "RightCorner");

        outsideRightCorner = settings.get<size_t>("Basic","outsideRightCorner");
        fillTile(outsideRightCorner, settings, "OutsideRightCorner");

        leftCorner = settings.get<size_t>("Basic","leftCorner");
        fillTile(leftCorner, settings, "LeftCorner");

        outsideLeftCorner = settings.get<size_t>("Basic","outsideLeftCorner");
        fillTile(outsideLeftCorner, settings, "OutsideLeftCorner");

        topCorner = settings.get<size_t>("Basic","topCorner");
        fillTile(topCorner, settings, "TopCorner");

        outsideTopCorner = settings.get<size_t>("Basic","outsideTopCorner");
        fillTile(outsideTopCorner, settings, "OutsideTopCorner");

        floor = settings.get<size_t>("Basic","floor");
        fillTile(floor, settings, "Floor");

        blank = settings.get<size_t>("Basic","blank");
        fillTile(blank, settings, "Blank");

        xDoor = settings.get<size_t>("Basic","xDoor");
        fillTile(xDoor, settings, "XDoor");

        yDoor = settings.get<size_t>("Basic","yDoor");
        fillTile(yDoor, settings, "YDoor");

        insideXWall = settings.get<size_t>("Basic","insideXWall");
        fillTile(insideXWall, settings, "InsideXWall");
        insideXWallEnd = settings.get<size_t>("Basic","insideXWallEnd");
        fillTile(insideXWallEnd, settings, "InsideXWallEnd");
        insideXWallEndBack = settings.get<size_t>("Basic","insideXWallEndBack");
        fillTile(insideXWallEndBack, settings, "InsideXWallEndBack");
        insideYWall = settings.get<size_t>("Basic","insideYWall");
        fillTile(insideYWall, settings, "InsideYWall");
        insideYWallEnd = settings.get<size_t>("Basic","insideYWallEnd");
        fillTile(insideYWallEnd, settings, "InsideYWallEnd");
        insideYWallEndBack = settings.get<size_t>("Basic","insideYWallEndBack");
        fillTile(insideYWallEndBack, settings, "InsideYWallEndBack");
        insideLeftCorner = settings.get<size_t>("Basic","insideLeftCorner");
        fillTile(insideLeftCorner, settings, "InsideLeftCorner");
        insideRightCorner = settings.get<size_t>("Basic","insideRightCorner");
        fillTile(insideRightCorner, settings, "InsideRightCorner");
        insideBottomCorner = settings.get<size_t>("Basic","insideBottomCorner");
        fillTile(insideBottomCorner, settings, "InsideBottomCorner");
        insideTopCorner = settings.get<size_t>("Basic","insideTopCorner");
        fillTile(insideTopCorner, settings, "InsideTopCorner");

        joinY = settings.get<size_t>("Basic","joinY");
        fillTile(joinY, settings, "JoinY");
        joinYRightCorner = settings.get<size_t>("Basic","joinYRightCorner");
        fillTile(joinYRightCorner, settings, "JoinYRightCorner");
        joinRightCorner = settings.get<size_t>("Basic","joinRightCorner");
        fillTile(joinRightCorner, settings, "JoinRightCorner");
        joinOutXRightCorner = settings.get<size_t>("Basic","joinOutXRightCorner");
        fillTile(joinOutXRightCorner, settings, "JoinOutXRightCorner");
        joinOutX = settings.get<size_t>("Basic","joinOutX");
        fillTile(joinOutX, settings, "JoinOutX");
        joinOutXTopCorner = settings.get<size_t>("Basic","joinOutXTopCorner");
        fillTile(joinOutXTopCorner, settings, "JoinOutXTopCorner");
        joinTopCorner = settings.get<size_t>("Basic","joinTopCorner");
        fillTile(joinTopCorner, settings, "JoinTopCorner");
        joinOutYTopCorner = settings.get<size_t>("Basic","joinOutYTopCorner");
        fillTile(joinOutYTopCorner, settings, "JoinOutYTopCorner");
        joinOutY = settings.get<size_t>("Basic","joinOutY");
        fillTile(joinOutY, settings, "JoinOutY");
        joinOutYLeftCorner = settings.get<size_t>("Basic","joinOutYLeftCorner");
        fillTile(joinOutYLeftCorner, settings, "JoinOutYLeftCorner");
        joinLeftCorner = settings.get<size_t>("Basic","joinLeftCorner");
        fillTile(joinLeftCorner, settings, "JoinLeftCorner");
        joinXLeftCorner = settings.get<size_t>("Basic","joinXLeftCorner");
        fillTile(joinXLeftCorner, settings, "JoinXLeftCorner");
        joinX = settings.get<size_t>("Basic","joinX");
        fillTile(joinX, settings, "JoinX");
        joinXBottomCorner = settings.get<size_t>("Basic","joinXBottomCorner");
        fillTile(joinXBottomCorner, settings, "JoinXBottomCorner");
        joinBottomCorner = settings.get<size_t>("Basic","joinBottomCorner");
        fillTile(joinBottomCorner, settings, "JoinBottomCorner");
        joinYBottomCorner = settings.get<size_t>("Basic","joinYBottomCorner");
        fillTile(joinYBottomCorner, settings, "JoinYBottomCorner");


        upStairs1 = settings.get<size_t>("Basic","upStairs1");
        upStairs2 = settings.get<size_t>("Basic","upStairs2");
        upStairs3 = settings.get<size_t>("Basic","upStairs3");

        upStairs4 = settings.get<size_t>("Basic","upStairs4");
        upStairs5 = settings.get<size_t>("Basic","upStairs5");
        upStairs6 = settings.get<size_t>("Basic","upStairs6");

        upStairs7 = settings.get<size_t>("Basic","upStairs7");
        upStairs8 = settings.get<size_t>("Basic","upStairs8");
        upStairs9 = settings.get<size_t>("Basic","upStairs9");
        
        downStairs1 = settings.get<size_t>("Basic","downStairs1");
        downStairs2 = settings.get<size_t>("Basic","downStairs2");
        downStairs3 = settings.get<size_t>("Basic","downStairs3");

        downStairs4 = settings.get<size_t>("Basic","downStairs4");
        downStairs5 = settings.get<size_t>("Basic","downStairs5");
        downStairs6 = settings.get<size_t>("Basic","downStairs6");

        downStairs7 = settings.get<size_t>("Basic","downStairs7");
        downStairs8 = settings.get<size_t>("Basic","downStairs8");
        downStairs9 = settings.get<size_t>("Basic","downStairs9");

        loadDoorMap(settings);
    }
    
    void TileSet::fillTile(size_t tile, Settings::Settings & settings, const std::string& section)
    {
        std::vector<std::pair<size_t, size_t> > tileVec;
        size_t normPercent = 100;

        if(settings.isSectionExists(section))
        {
            Settings::Container properties = settings.getPropertiesInSection(section);
            size_t size = properties.size();

            for(size_t i = 0 ; i < size ; i++)
            {
                if(properties[i] != "normal")
                {
                    std::stringstream buffer(properties[i]);
                    size_t propertyAsSizeT;
                    buffer >> propertyAsSizeT;

                    size_t value = settings.get<size_t>(section, properties[i]);
                    tileVec.push_back(std::pair<size_t, size_t>(propertyAsSizeT, value));
                }
            }

            normPercent = settings.get<size_t>(section, "normal");
        }

        mAlternatives[tile] = std::pair<std::vector<std::pair<size_t, size_t> >, size_t>(tileVec, normPercent);
    }
    
    size_t TileSet::getRandomTile(size_t tile)
    {
        if(mAlternatives.find(tile) == mAlternatives.end())
            return tile;

        std::vector<std::pair<size_t, size_t> >& tileVec = mAlternatives[tile].first;
        size_t normPercent = mAlternatives[tile].second;

        size_t random = randomInRange(0, 100);

        if(random <= normPercent)
            return tile;
        
        size_t max = 0;
        for(size_t i = 0; i < tileVec.size(); i++)
            max += tileVec[i].second;
       
        random = randomInRange(0, max);          
        
        size_t i = 0;
        for(; i < tileVec.size() && random > tileVec[i].second; i++) 
            random -= tileVec[i].second;

        return tileVec[i].first;
    }

    std::map<size_t, size_t> TileSet::getDoorMap()
    {
        return mDoorMap;
    }
    
    void TileSet::loadDoorMap(Settings::Settings & settings)
    {
        Settings::Container properties = settings.getPropertiesInSection("DoorMap");

        for(Settings::Container::const_iterator it = properties.begin(); it != properties.end(); ++it)
        {
            std::stringstream buffer(*it);
            size_t propertyAsSizeT;
            buffer >> propertyAsSizeT;

            size_t value = settings.get<size_t>("DoorMap", *it);

            mDoorMap[propertyAsSizeT] = value;
            mDoorMap[value] = propertyAsSizeT;
        }
    }

    size_t TileSet::convert(TileSetEnum::TileSetEnum val)
    {
        switch(val)
        {
            case TileSetEnum::xWall: return xWall;
            case TileSetEnum::outsideXWall: return outsideXWall;
            case TileSetEnum::yWall: return yWall;
            case TileSetEnum::outsideYWall: return outsideYWall;
            case TileSetEnum::bottomCorner: return bottomCorner;
            case TileSetEnum::outsideBottomCorner: return outsideBottomCorner;
            case TileSetEnum::rightCorner: return rightCorner;
            case TileSetEnum::outsideRightCorner: return outsideRightCorner;
            case TileSetEnum::leftCorner: return leftCorner;
            case TileSetEnum::outsideLeftCorner: return outsideLeftCorner;
            case TileSetEnum::topCorner: return topCorner;
            case TileSetEnum::outsideTopCorner: return outsideTopCorner;
            case TileSetEnum::floor: return floor;
            case TileSetEnum::blank: return blank;
            case TileSetEnum::xDoor: return xDoor;
            case TileSetEnum::yDoor: return yDoor;

            case TileSetEnum::upStairs1: return upStairs1;
            case TileSetEnum::upStairs2: return upStairs2;
            case TileSetEnum::upStairs3: return upStairs3;

            case TileSetEnum::upStairs4: return upStairs4;
            case TileSetEnum::upStairs5: return upStairs5;
            case TileSetEnum::upStairs6: return upStairs6;

            case TileSetEnum::upStairs7: return upStairs7;
            case TileSetEnum::upStairs8: return upStairs8;
            case TileSetEnum::upStairs9: return upStairs9;

            case TileSetEnum::downStairs1: return downStairs1;
            case TileSetEnum::downStairs2: return downStairs2;
            case TileSetEnum::downStairs3: return downStairs3;

            case TileSetEnum::downStairs4: return downStairs4;
            case TileSetEnum::downStairs5: return downStairs5;
            case TileSetEnum::downStairs6: return downStairs6;

            case TileSetEnum::downStairs7: return downStairs7;
            case TileSetEnum::downStairs8: return downStairs8;
            case TileSetEnum::downStairs9: return downStairs9;

            case TileSetEnum::insideXWall: return insideXWall;
            case TileSetEnum::insideXWallEnd: return insideXWallEnd;
            case TileSetEnum::insideXWallEndBack: return insideXWallEndBack;
            case TileSetEnum::insideYWall: return insideYWall;
            case TileSetEnum::insideYWallEnd: return insideYWallEnd;
            case TileSetEnum::insideYWallEndBack: return insideYWallEndBack;
            case TileSetEnum::insideLeftCorner: return insideLeftCorner;
            case TileSetEnum::insideRightCorner: return insideRightCorner;
            case TileSetEnum::insideBottomCorner: return insideBottomCorner;
            case TileSetEnum::insideTopCorner: return insideTopCorner;

            case TileSetEnum::joinY: return joinY;
            case TileSetEnum::joinYRightCorner: return joinYRightCorner;
            case TileSetEnum::joinRightCorner: return joinRightCorner;
            case TileSetEnum::joinOutXRightCorner: return joinOutXRightCorner;
            case TileSetEnum::joinOutX: return joinOutX;
            case TileSetEnum::joinOutXTopCorner: return joinOutXTopCorner;
            case TileSetEnum::joinTopCorner: return joinTopCorner;
            case TileSetEnum::joinOutYTopCorner: return joinOutYTopCorner;
            case TileSetEnum::joinOutY: return joinOutY;
            case TileSetEnum::joinOutYLeftCorner: return joinOutYLeftCorner;
            case TileSetEnum::joinLeftCorner: return joinLeftCorner;
            case TileSetEnum::joinXLeftCorner: return joinXLeftCorner;
            case TileSetEnum::joinX: return joinX;
            case TileSetEnum::joinXBottomCorner: return joinXBottomCorner;
            case TileSetEnum::joinBottomCorner: return joinBottomCorner;
            case TileSetEnum::joinYBottomCorner: return joinYBottomCorner;
        }

        return val;
    }
}
