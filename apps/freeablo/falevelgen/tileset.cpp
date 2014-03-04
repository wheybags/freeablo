#include "tileset.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <faio/faio.h>

namespace FALevelGen
{
    namespace bpt = boost::property_tree;

    TileSet::TileSet(const std::string& path)
    {
        FAIO::FAFile* file = FAIO::FAfopen(path);

        size_t size = FAIO::FAsize(file);
        char* str = new char[size+1];

        FAIO::FAfread(str, 1, size, file);
        str[size] = '\0';

        std::stringstream s;
        s << str;

        bpt::ptree pt;
        bpt::read_ini(s, pt);

        delete[] str;

        xWall = pt.get<size_t>("Basic.xWall");
        outsideXWall = pt.get<size_t>("Basic.outsideXWall");
        yWall = pt.get<size_t>("Basic.yWall");
        outsideYWall = pt.get<size_t>("Basic.outsideYWall");
        bottomCorner = pt.get<size_t>("Basic.bottomCorner");
        outsideBottomCorner = pt.get<size_t>("Basic.outsideBottomCorner");
        rightCorner = pt.get<size_t>("Basic.rightCorner");
        outsideRightCorner = pt.get<size_t>("Basic.outsideRightCorner");
        leftCorner = pt.get<size_t>("Basic.leftCorner");
        outsideLeftCorner = pt.get<size_t>("Basic.outsideLeftCorner");
        topCorner = pt.get<size_t>("Basic.topCorner");
        outsideTopCorner = pt.get<size_t>("Basic.outsideTopCorner");
        floor = pt.get<size_t>("Basic.floor");
        blank = pt.get<size_t>("Basic.blank");
        xWallEnd = pt.get<size_t>("Basic.xWallEnd");
        yWallEnd = pt.get<size_t>("Basic.yWallEnd");
        xDoor = pt.get<size_t>("Basic.xDoor");
        yDoor = pt.get<size_t>("Basic.yDoor");
    }
}
