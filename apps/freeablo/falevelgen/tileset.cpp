#include "tileset.h"

#include <sstream>
#include <algorithm>

#include <faio/faio.h>
#include <misc/fareadini.h>

#include "random.h"

namespace FALevelGen
{
    namespace bpt = boost::property_tree;

    TileSet::TileSet(const std::string& path)
    {
        bpt::ptree pt;
        Misc::readIni(path, pt);

        xWall = pt.get<size_t>("Basic.xWall");
        fillTile(xWall, pt, "XWall");
        
        outsideXWall = pt.get<size_t>("Basic.outsideXWall");
        fillTile(outsideXWall, pt, "outsideXWall");

        yWall = pt.get<size_t>("Basic.yWall");
        fillTile(yWall, pt, "YWall");

        outsideYWall = pt.get<size_t>("Basic.outsideYWall");
        fillTile(outsideYWall, pt, "OutsideYWall");

        bottomCorner = pt.get<size_t>("Basic.bottomCorner");
        fillTile(bottomCorner, pt, "BottomCorner");

        outsideBottomCorner = pt.get<size_t>("Basic.outsideBottomCorner");
        fillTile(outsideBottomCorner, pt, "OutsideBottomCorner");

        rightCorner = pt.get<size_t>("Basic.rightCorner");
        fillTile(rightCorner, pt, "RightCorner");

        outsideRightCorner = pt.get<size_t>("Basic.outsideRightCorner");
        fillTile(outsideRightCorner, pt, "OutsideRightCorner");

        leftCorner = pt.get<size_t>("Basic.leftCorner");
        fillTile(leftCorner, pt, "LeftCorner");

        outsideLeftCorner = pt.get<size_t>("Basic.outsideLeftCorner");
        fillTile(outsideLeftCorner, pt, "OutsideLeftCorner");

        topCorner = pt.get<size_t>("Basic.topCorner");
        fillTile(topCorner, pt, "TopCorner");

        outsideTopCorner = pt.get<size_t>("Basic.outsideTopCorner");
        fillTile(outsideTopCorner, pt, "OutsideTopCorner");

        floor = pt.get<size_t>("Basic.floor");
        fillTile(floor, pt, "Floor");

        blank = pt.get<size_t>("Basic.blank");
        fillTile(blank, pt, "Blank");

        xDoor = pt.get<size_t>("Basic.xDoor");
        fillTile(xDoor, pt, "XDoor");

        yDoor = pt.get<size_t>("Basic.yDoor");
        fillTile(yDoor, pt, "YDoor");

        insideXWall = pt.get<size_t>("Basic.insideXWall");
        fillTile(insideXWall, pt, "InsideXWall");
        insideXWallEnd = pt.get<size_t>("Basic.insideXWallEnd");
        fillTile(insideXWallEnd, pt, "InsideXWallEnd");
        insideXWallEndBack = pt.get<size_t>("Basic.insideXWallEndBack");
        fillTile(insideXWallEndBack, pt, "InsideXWallEndBack");
        insideYWall = pt.get<size_t>("Basic.insideYWall");
        fillTile(insideYWall, pt, "InsideYWall");
        insideYWallEnd = pt.get<size_t>("Basic.insideYWallEnd");
        fillTile(insideYWallEnd, pt, "InsideYWallEnd");
        insideYWallEndBack = pt.get<size_t>("Basic.insideYWallEndBack");
        fillTile(insideYWallEndBack, pt, "InsideYWallEndBack");
        insideLeftCorner = pt.get<size_t>("Basic.insideLeftCorner");
        fillTile(insideLeftCorner, pt, "InsideLeftCorner");
        insideRightCorner = pt.get<size_t>("Basic.insideRightCorner");
        fillTile(insideRightCorner, pt, "InsideRightCorner");
        insideBottomCorner = pt.get<size_t>("Basic.insideBottomCorner");
        fillTile(insideBottomCorner, pt, "InsideBottomCorner");
        insideTopCorner = pt.get<size_t>("Basic.insideTopCorner");
        fillTile(insideTopCorner, pt, "InsideTopCorner");

        joinY = pt.get<size_t>("Basic.joinY");
        fillTile(joinY, pt, "JoinY");
        joinYRightCorner = pt.get<size_t>("Basic.joinYRightCorner");
        fillTile(joinYRightCorner, pt, "JoinYRightCorner");
        joinRightCorner = pt.get<size_t>("Basic.joinRightCorner");
        fillTile(joinRightCorner, pt, "JoinRightCorner");
        joinOutXRightCorner = pt.get<size_t>("Basic.joinOutXRightCorner");
        fillTile(joinOutXRightCorner, pt, "JoinOutXRightCorner");
        joinOutX = pt.get<size_t>("Basic.joinOutX");
        fillTile(joinOutX, pt, "JoinOutX");
        joinOutXTopCorner = pt.get<size_t>("Basic.joinOutXTopCorner");
        fillTile(joinOutXTopCorner, pt, "JoinOutXTopCorner");
        joinTopCorner = pt.get<size_t>("Basic.joinTopCorner");
        fillTile(joinTopCorner, pt, "JoinTopCorner");
        joinOutYTopCorner = pt.get<size_t>("Basic.joinOutYTopCorner");
        fillTile(joinOutYTopCorner, pt, "JoinOutYTopCorner");
        joinOutY = pt.get<size_t>("Basic.joinOutY");
        fillTile(joinOutY, pt, "JoinOutY");
        joinOutYLeftCorner = pt.get<size_t>("Basic.joinOutYLeftCorner");
        fillTile(joinOutYLeftCorner, pt, "JoinOutYLeftCorner");
        joinLeftCorner = pt.get<size_t>("Basic.joinLeftCorner");
        fillTile(joinLeftCorner, pt, "JoinLeftCorner");
        joinXLeftCorner = pt.get<size_t>("Basic.joinXLeftCorner");
        fillTile(joinXLeftCorner, pt, "JoinXLeftCorner");
        joinX = pt.get<size_t>("Basic.joinX");
        fillTile(joinX, pt, "JoinX");
        joinXBottomCorner = pt.get<size_t>("Basic.joinXBottomCorner");
        fillTile(joinXBottomCorner, pt, "JoinXBottomCorner");
        joinBottomCorner = pt.get<size_t>("Basic.joinBottomCorner");
        fillTile(joinBottomCorner, pt, "JoinBottomCorner");
        joinYBottomCorner = pt.get<size_t>("Basic.joinYBottomCorner");
        fillTile(joinYBottomCorner, pt, "JoinYBottomCorner");


        upStairs1 = pt.get<size_t>("Basic.upStairs1");
        upStairs2 = pt.get<size_t>("Basic.upStairs2");
        upStairs3 = pt.get<size_t>("Basic.upStairs3");

        upStairs4 = pt.get<size_t>("Basic.upStairs4");
        upStairs5 = pt.get<size_t>("Basic.upStairs5");
        upStairs6 = pt.get<size_t>("Basic.upStairs6");

        upStairs7 = pt.get<size_t>("Basic.upStairs7");
        upStairs8 = pt.get<size_t>("Basic.upStairs8");
        upStairs9 = pt.get<size_t>("Basic.upStairs9");
        
        downStairs1 = pt.get<size_t>("Basic.downStairs1");
        downStairs2 = pt.get<size_t>("Basic.downStairs2");
        downStairs3 = pt.get<size_t>("Basic.downStairs3");

        downStairs4 = pt.get<size_t>("Basic.downStairs4");
        downStairs5 = pt.get<size_t>("Basic.downStairs5");
        downStairs6 = pt.get<size_t>("Basic.downStairs6");

        downStairs7 = pt.get<size_t>("Basic.downStairs7");
        downStairs8 = pt.get<size_t>("Basic.downStairs8");
        downStairs9 = pt.get<size_t>("Basic.downStairs9");

        loadDoorMap(pt);
    }
    
    void TileSet::fillTile(size_t tile, bpt::ptree& pt, const std::string& str)
    {
        std::vector<std::pair<size_t, size_t> > tileVec;
        size_t normPercent = 100;
        bpt::ptree::assoc_iterator tileIt = pt.find(str); 

        if(tileIt != pt.not_found())
        {
            for(bpt::ptree::const_iterator key = tileIt->second.begin(); key != tileIt->second.end(); ++key)
            {
                if(key->first != "normal")
                {
                    std::stringstream buffer(key->first);
                    size_t first;
                    buffer >> first;

                    tileVec.push_back(std::pair<size_t, size_t>(first, key->second.get_value<size_t>()));
                }
            }
            normPercent = tileIt->second.get<size_t>("normal");
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
    
    void TileSet::loadDoorMap(bpt::ptree& pt)
    {
        bpt::ptree::assoc_iterator doorMapIt = pt.find("DoorMap");

        for(bpt::ptree::const_iterator key = doorMapIt->second.begin(); key != doorMapIt->second.end(); ++key)
        {
            std::stringstream buffer(key->first);
            size_t first;
            buffer >> first;

            size_t second = key->second.get_value<size_t>();

            mDoorMap[first] = second;
            mDoorMap[second] = first;
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
