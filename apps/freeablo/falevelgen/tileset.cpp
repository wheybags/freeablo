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

        xWallEnd = pt.get<size_t>("Basic.xWallEnd");
        fillTile(xWallEnd, pt, "XWallEnd");

        yWallEnd = pt.get<size_t>("Basic.yWallEnd");
        fillTile(yWallEnd, pt, "YWallEnd");

        xDoor = pt.get<size_t>("Basic.xDoor");
        fillTile(xDoor, pt, "XDoor");

        yDoor = pt.get<size_t>("Basic.yDoor");
        fillTile(yDoor, pt, "YDoor");

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
}
