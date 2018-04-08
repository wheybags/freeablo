#include "levelgen.h"
#include "../faworld/actor.h"
#include "misc/random.h"
#include "mst.h"
#include "tileset.h"
#include <algorithm>
#include <cmath>
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include <misc/assert.h>
#include <misc/misc.h>
#include <sstream>
#include <stdlib.h>
#include <vector>

namespace FALevelGen
{
    class Room
    {
    public:
        int32_t xPos;
        int32_t yPos;
        int32_t width;
        int32_t height;

        Room(int32_t _xPos, int32_t _yPos, int32_t _width, int32_t _height) : xPos(_xPos), yPos(_yPos), width(_width), height(_height) {}

        bool intersects(const Room& other) const
        {
            return !(yPos + height <= other.yPos + 1 || yPos >= other.yPos + other.height - 1 || xPos + width <= other.xPos + 1 ||
                     xPos >= other.xPos + other.width - 1);
        }

        bool onBorder(int32_t xCoord, int32_t yCoord)
        {
            // Draw x oriented walls
            for (int32_t x = 0; x < width; x++)
            {
                if ((xCoord == x + xPos && yCoord == yPos) || (xCoord == x + xPos && yCoord == height - 1 + yPos))
                    return true;
                // level[x + room.xPos][room.yPos] = wall;
                // level[x + room.xPos][room.height-1 + room.yPos] = wall;
            }

            // Draw y oriented walls
            for (int32_t y = 0; y < height; y++)
            {
                if ((xCoord == xPos && yCoord == y + yPos) || (xCoord == width - 1 + xPos && yCoord == y + yPos))
                    return true;

                // level[room.xPos][y + room.yPos] = wall;
                // level[room.width-1 + room.xPos][y + room.yPos] = wall;
            }

            return false;
        }

        std::pair<int32_t, int32_t> centre() const { return std::pair<int32_t, int32_t>(xPos + (width / 2), yPos + (height / 2)); }

        int32_t area() const { return width * height; }

        int32_t distance(const Room& other) const
        {
            return static_cast<int32_t>(sqrt(static_cast<float>((centre().first - other.centre().first) * (centre().first - other.centre().first) +
                                                                (centre().second - other.centre().second) * (centre().second - other.centre().second))));
        }
    };

    // the values here are not significant, these were just conventient when debugging level 3
    // they must only be distinct
    enum class Basic : int32_t
    {
        insideWall = 27,
        wall = 56,
        upStairs = 35,
        downStairs = 34,
        door = 67,
        floor = 116,
        blank = 115
    };

    void fillRoom(const Room& room, Level::Dun& level)
    {
        for (int32_t x = 0; x < room.width; x++)
        {
            for (int32_t y = 0; y < room.height; y++)
            {
                level.get(x + room.xPos, y + room.yPos) = (int32_t)Basic::floor;
            }
        }
    }

    void drawCorridoorSegment(const Room& room, const std::vector<Room>& corridoorRooms, Level::Dun& level)
    {
        fillRoom(room, level);

        for (int32_t i = 0; i < (int32_t)corridoorRooms.size(); i++)
        {
            if (room.intersects(corridoorRooms[i]))
                fillRoom(corridoorRooms[i], level);
        }
    }

    // Ensures that two points are connected by inserting an l-shaped corridoor,
    // also draws any rooms in rooms vector that the corridoor intersects
    void connect(const Room& a, const Room& b, const std::vector<Room>& rooms, Level::Dun& level)
    {
        int32_t ax = a.centre().first;
        int32_t ay = a.centre().second;

        int32_t bx = b.centre().first;
        int32_t by = b.centre().second;

        if (bx != ax)
        {
            if (bx > ax)
            {
                Room x(ax, ay - 1, bx - ax, 3);
                drawCorridoorSegment(x, rooms, level);
            }
            else
            {
                Room x(bx, ay - 1, ax - bx, 3);
                drawCorridoorSegment(x, rooms, level);
            }
        }
        if (by != ay)
        {
            if (by > ay)
            {
                Room y(bx - 1, ay, 3, by - ay);
                drawCorridoorSegment(y, rooms, level);
            }
            else
            {
                Room y(bx - 1, by, 3, ay - by + 2);
                drawCorridoorSegment(y, rooms, level);
            }
        }
    }

    // Move room in direction specified by normalised vector, making sure to keep within
    // grid of size width * height
    void moveRoom(Room& room, const std::pair<float, float>& vector, int32_t width, int32_t height)
    {
        int32_t newX = 0, newY = 0;
        std::tie(newX, newY) = Misc::getNextPosByDir({room.xPos, room.yPos}, Misc::getVecDir(vector));

        // Make sure not to move outside map
        if (newX >= 1 && newY >= 1 && newX + room.width < width - 1 && newY + room.height < height - 1)
        {
            room.xPos = newX;
            room.yPos = newY;
        }
    }

    void normalise(std::pair<float, float>& vector)
    {
        float magnitude = sqrt(vector.first * vector.first + vector.second * vector.second);
        vector.first /= (float)magnitude;
        vector.second /= (float)magnitude;
    }

    // Removes the room overlapping the largest number of rooms repeatedly,
    // until there are no overlaps
    void removeOverlaps(std::vector<Room>& rooms)
    {
        bool overlap = true;

        while (overlap)
        {
            overlap = false;

            int32_t maxIndex = -1;
            int32_t maxNeighbourCount = 0;

            for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
            {
                int32_t neighbourCount = 0;

                for (int32_t j = 0; j < (int32_t)rooms.size(); j++)
                {
                    if (i != j && rooms[i].intersects(rooms[j]))
                    {
                        neighbourCount++;
                        overlap = true;
                    }
                }

                if (neighbourCount > maxNeighbourCount)
                {
                    maxIndex = i;
                    maxNeighbourCount = neighbourCount;
                }
            }

            if (maxIndex > -1)
                rooms.erase(rooms.begin() + maxIndex);
        }
    }

    // Separate rooms so they don't overlap, using flocking ai
    // based on the algorithm described here:
    // http://gamedevelopment.tutsplus.com/tutorials/the-three-simple-rules-of-flocking-behaviors-alignment-cohesion-and-separation--gamedev-3444
    void separate(std::vector<Room>& rooms, int32_t width, int32_t height)
    {
        bool overlap = true;

        int its = 0;

        while (its < 400 && overlap)
        {
            its++;

            overlap = false;

            for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
            {
                std::pair<float, float> vector(0.f, 0.f);

                std::pair<int32_t, int32_t> currentCentre = rooms[i].centre();

                int32_t neighbourCount = 0;

                for (int32_t j = 0; j < (int32_t)rooms.size(); j++)
                {
                    if (i == j)
                        continue;

                    bool intersects = rooms[i].intersects(rooms[j]);

                    overlap = overlap || intersects;

                    if (intersects)
                    {
                        std::pair<int32_t, int32_t> centre = rooms[j].centre();

                        if (centre.first == currentCentre.first && centre.second == currentCentre.second)
                        {
                            vector.first = static_cast<float>(Random::randomInRange(0, 10));
                            vector.second = static_cast<float>(Random::randomInRange(0, 10));
                            neighbourCount++;
                            continue;
                        }

                        std::pair<float, float> iToJ = Misc::getVec(currentCentre, centre);
                        normalise(iToJ);

                        vector.first += iToJ.first * rooms[i].distance(rooms[j]);
                        vector.second += iToJ.second * rooms[i].distance(rooms[j]);

                        neighbourCount++;
                    }
                }

                if (vector.first == 0 && vector.second == 0)
                    continue;

                vector.first /= (float)neighbourCount;
                vector.second /= (float)neighbourCount;

                normalise(vector);

                // invert
                vector.first *= -1;
                vector.second *= -1;

                moveRoom(rooms[i], vector, width, height);
            }
        }

        if (overlap)
            removeOverlaps(rooms);
    }

    void generateRooms(std::vector<Room>& rooms, int32_t width, int32_t height)
    {
        int32_t maxDimension = 10;

        int32_t placed = 0;

        int32_t centreX = width / 2;
        int32_t centreY = height / 2;

        // The following two are based on the fact that 150 rooms in a radius of
        // 15 looks good on an 85*75 map
        int32_t numRooms = static_cast<int32_t>(std::min(width, height) * (150.0 / 75.0));
        int32_t radius = static_cast<int32_t>(std::min(width, height) * (15.0 / 75.0));

        while (placed < numRooms)
        {
            Room newRoom(Random::randomInRange(0, width - 4), Random::randomInRange(0, height - 4), 0, 0);

            if (((centreX - newRoom.centre().first) * (centreX - newRoom.centre().first) +
                 (centreY - newRoom.centre().second) * (centreY - newRoom.centre().second)) > radius * radius)
                continue;

            newRoom.width = Random::normRand(4, std::min(width - newRoom.xPos, maxDimension));
            newRoom.height = Random::normRand(4, std::min(height - newRoom.yPos, maxDimension));

            float ratio = ((float)newRoom.width) / ((float)newRoom.height);

            if (ratio < 0.5 || ratio > 2.0)
                continue;

            placed++;
            rooms.push_back(newRoom);
        }

        separate(rooms, width, height);
    }

    void drawRoom(const Room& room, Level::Dun& level)
    {
        // Draw x oriented walls
        for (int32_t x = 0; x < room.width; x++)
        {
            level.get(x + room.xPos, room.yPos) = (int32_t)Basic::wall;
            level.get(x + room.xPos, room.height - 1 + room.yPos) = (int32_t)Basic::wall;
        }

        // Draw y oriented walls
        for (int32_t y = 0; y < room.height; y++)
        {
            level.get(room.xPos, y + room.yPos) = (int32_t)Basic::wall;
            level.get(room.width - 1 + room.xPos, y + room.yPos) = (int32_t)Basic::wall;
        }

        // Fill ground
        for (int32_t x = 1; x < room.width - 1; x++)
        {
            for (int32_t y = 1; y < room.height - 1; y++)
            {
                level.get(x + room.xPos, y + room.yPos) = (int32_t)Basic::floor;
            }
        }
    }

    // Get the value at (x,y) in level, or zero if it is an invalid position
    int32_t getXY(int32_t x, int32_t y, const Level::Dun& level)
    {
        if (x < 0 || x >= (int32_t)level.width() || y < 0 || y >= (int32_t)level.height())
            return 0;

        return level.get(x, y);
    }

    // Returns true if the tile at (x,y) in level borders any tile of the value tile,
    // false otherwise
    bool borders(int32_t x, int32_t y, Basic tile, const Level::Dun& level)
    {
        for (int32_t xoffs = -1; xoffs < 2; xoffs++)
        {
            for (int32_t yoffs = -1; yoffs < 2; yoffs++)
            {
                int32_t testX = xoffs + x;
                int32_t testY = yoffs + y;

                if (getXY(testX, testY, level) == static_cast<int32_t>(tile))
                    return true;
            }
        }

        return false;
    }

    bool addWalls(Level::Dun& level)
    {
        bool retval = false;

        // Bound corridoors with walls
        for (int32_t x = 0; x < level.width(); x++)
        {
            for (int32_t y = 0; y < level.height(); y++)
            {
                if (getXY(x, y, level) == (int32_t)Basic::blank && borders(x, y, Basic::floor, level))
                {
                    level.get(x, y) = (int32_t)Basic::wall;
                    retval = true;
                }
            }
        }

        return retval;
    }

    bool cleanupHelper(Level::Dun& level, std::vector<Room>& rooms, bool includeBorders)
    {
        bool retval = false;

        for (int32_t x = 0; x < (int32_t)level.width(); x++)
        {
            for (int32_t y = 0; y < (int32_t)level.height(); y++)
            {
                if (level.get(x, y) != (int32_t)Basic::wall || (!includeBorders && borders(x, y, Basic::blank, level)))
                    continue;

                bool fixExternalT = false;

                if (includeBorders && borders(x, y, Basic::blank, level))
                {
                    int wallCount = 0;

                    if (level.get(x + 1, y) == (int32_t)Basic::wall && borders(x + 1, y, Basic::blank, level))
                        wallCount++;
                    if (level.get(x - 1, y) == (int32_t)Basic::wall && borders(x - 1, y, Basic::blank, level))
                        wallCount++;
                    if (level.get(x, y + 1) == (int32_t)Basic::wall && borders(x, y + 1, Basic::blank, level))
                        wallCount++;
                    if (level.get(x, y - 1) == (int32_t)Basic::wall && borders(x, y - 1, Basic::blank, level))
                        wallCount++;

                    fixExternalT = wallCount > 2;
                }

                if ((level.get(x, y + 1) == (int32_t)Basic::wall && level.get(x + 1, y) == (int32_t)Basic::wall &&
                     level.get(x + 1, y + 1) == (int32_t)Basic::wall) ||
                    fixExternalT)
                {
                    level.get(x, y) = (int32_t)Basic::floor;
                    retval = true;

                    for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
                    {
                        if (rooms[i].onBorder(x, y))
                        {
                            // Draw x oriented walls
                            for (int32_t i_x = 0; i_x < rooms[i].width; i_x++)
                            {
                                if (includeBorders || !borders(i_x + rooms[i].xPos, rooms[i].yPos, Basic::blank, level))
                                    level.get(i_x + rooms[i].xPos, rooms[i].yPos) = (int32_t)Basic::floor;
                                if (includeBorders || !borders(i_x + rooms[i].xPos, rooms[i].height - 1 + rooms[i].yPos, Basic::blank, level))
                                    level.get(i_x + rooms[i].xPos, rooms[i].height - 1 + rooms[i].yPos) = (int32_t)Basic::floor;
                            }

                            // Draw y oriented walls
                            for (int32_t i_y = 0; i_y < rooms[i].height; i_y++)
                            {
                                if (includeBorders || !borders(rooms[i].xPos, i_y + rooms[i].yPos, Basic::blank, level))
                                    level.get(rooms[i].xPos, i_y + rooms[i].yPos) = (int32_t)Basic::floor;
                                if (includeBorders || !borders(rooms[i].width - 1 + rooms[i].xPos, i_y + rooms[i].yPos, Basic::blank, level))
                                    level.get(rooms[i].width - 1 + rooms[i].xPos, i_y + rooms[i].yPos) = (int32_t)Basic::floor;
                            }

                            rooms.erase(rooms.begin() + i);
                        }
                    }
                }
            }
        }

        return retval;
    }

    bool isWall(int32_t x, int32_t y, const Level::Dun& level, bool inside)
    {
        if (inside)
            return getXY(x, y, level) == (int32_t)Basic::insideWall || getXY(x, y, level) == (int32_t)Basic::door;
        else
            return getXY(x, y, level) == (int32_t)Basic::wall || getXY(x, y, level) == (int32_t)Basic::upStairs;
    }

    bool cleanLooseWallsHelper(int32_t x, int32_t y, const Level::Dun& level, bool wallsSeparated)
    {
        if (wallsSeparated)
            return getXY(x, y, level) == (int32_t)Basic::insideWall || getXY(x, y, level) == (int32_t)Basic::door;
        else
            return getXY(x, y, level) == (int32_t)Basic::wall || getXY(x, y, level) == (int32_t)Basic::door;
    }

    void cleanLooseWalls(Level::Dun& level, bool wallsSeparated)
    {
        // Remove any isolated wall blocks
        for (int32_t x = 0; x < (int32_t)level.width(); x++)
        {
            for (int32_t y = 0; y < (int32_t)level.height(); y++)
            {
                if (cleanLooseWallsHelper(x, y, level, wallsSeparated) && (!cleanLooseWallsHelper(x + 1, y, level, wallsSeparated)) &&
                    (!cleanLooseWallsHelper(x - 1, y, level, wallsSeparated)) && (!cleanLooseWallsHelper(x, y + 1, level, wallsSeparated)) &&
                    (!cleanLooseWallsHelper(x, y - 1, level, wallsSeparated)))
                {
                    level.get(x, y) = (int32_t)Basic::floor;
                }
            }
        }
    }

    // Remove double walls, as the tileset does not allow for them
    void cleanup(Level::Dun& level, std::vector<Room>& rooms)
    {
        bool dirty = cleanupHelper(level, rooms, false);

        // iterate until there are no double walls at all
        while (dirty)
        {
            dirty = cleanupHelper(level, rooms, true);

            if (dirty)
                addWalls(level); // cleanupHelper may leave floor blocks directly beside blank blocks
        }

        cleanLooseWalls(level, false);
    }

    // Helper function for adding doors
    // Iterates over all blocks on a wall, and adds doors where necessary, looking at what is in the direction
    // indicated by add (1 or -1) to determine if a door is needed
    void doorAddHelper(Level::Dun& level, int32_t otherCoord, int32_t add, int32_t start, int32_t end, bool xAxis, int32_t levelNum)
    {
        std::vector<std::pair<int32_t, int32_t>> region;
        bool connected = false;
        bool hole = false;

        for (int32_t i = start; i < end; i++)
        {
            if ((xAxis && (getXY(i, otherCoord, level) == (int32_t)Basic::floor || getXY(i, otherCoord, level) == (int32_t)Basic::door)) ||
                (!xAxis && (getXY(otherCoord, i, level) == (int32_t)Basic::floor || getXY(otherCoord, i, level) == (int32_t)Basic::door)))
            {
                hole = true;
            }
            else if ((xAxis && getXY(i, otherCoord + add, level) != (int32_t)Basic::floor) ||
                     (!xAxis && getXY(otherCoord + add, i, level) != (int32_t)Basic::floor))
            {
                if (hole)
                    region.resize(0);
                hole = false;
            }

            if (!hole)
            {
                if ((xAxis && getXY(i, otherCoord + add, level) == (int32_t)Basic::floor) ||
                    (!xAxis && getXY(otherCoord + add, i, level) == (int32_t)Basic::floor))
                {
                    if (!connected)
                    {
                        if (region.size() > 0)
                            level.get(region[region.size() / 2].first, region[region.size() / 2].second) = (int32_t)Basic::door;

                        region.resize(0);
                        connected = true;
                    }

                    if (xAxis)
                    {
                        if (getXY(i - 1, otherCoord, level) == (int32_t)Basic::wall && getXY(i + 1, otherCoord, level) == (int32_t)Basic::wall)

                            region.push_back(std::pair<int32_t, int32_t>(i, otherCoord));
                    }
                    else
                    {
                        if (getXY(otherCoord, i - 1, level) == (int32_t)Basic::wall && getXY(otherCoord, i + 1, level) == (int32_t)Basic::wall)

                            region.push_back(std::pair<int32_t, int32_t>(otherCoord, i));
                    }
                }
                else
                {
                    connected = false;
                }
            }
        }

        if (!hole && region.size() > 0)
            level.get(region[region.size() / 2].first, region[region.size() / 2].second) = (levelNum == 4) ? (int32_t)Basic::floor : (int32_t)Basic::door;
    }

    void addDoors(Level::Dun& level, const std::vector<Room>& rooms, int32_t levelNum)
    {
        for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
        {
            // Top x wall
            doorAddHelper(level, rooms[i].yPos, -1, rooms[i].xPos + 1, rooms[i].xPos + rooms[i].width - 1, true, levelNum);
            // Bottom x wall
            doorAddHelper(level, rooms[i].yPos + rooms[i].height - 1, +1, rooms[i].xPos + 1, rooms[i].xPos + rooms[i].width - 1, true, levelNum);

            // Left y wall
            doorAddHelper(level, rooms[i].xPos, -1, rooms[i].yPos + 1, rooms[i].yPos + rooms[i].height - 1, false, levelNum);
            // Right y wall
            doorAddHelper(level, rooms[i].xPos + rooms[i].width - 1, +1, rooms[i].yPos + 1, rooms[i].yPos + rooms[i].height - 1, false, levelNum);
        }
    }

    bool placeUpStairs(Level::Dun& level, const std::vector<Room>& rooms, int32_t levelNum)
    {
        if (levelNum == 1 || levelNum == 3)
        {
            for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
            {
                int32_t baseX = rooms[i].xPos + (rooms[i].width / 2);
                int32_t baseY = rooms[i].yPos;

                // on a wall
                if (level.get(baseX - 1, baseY - 2) == (int32_t)Basic::blank && level.get(baseX, baseY - 2) == (int32_t)Basic::blank &&
                    level.get(baseX + 1, baseY - 2) == (int32_t)Basic::blank && level.get(baseX - 1, baseY - 1) == (int32_t)Basic::blank &&
                    level.get(baseX, baseY - 1) == (int32_t)Basic::blank && level.get(baseX + 1, baseY - 1) == (int32_t)Basic::blank &&
                    level.get(baseX - 1, baseY) == (int32_t)Basic::wall && level.get(baseX, baseY) == (int32_t)Basic::wall &&
                    level.get(baseX + 1, baseY) == (int32_t)Basic::wall && level.get(baseX - 1, baseY + 1) == (int32_t)Basic::floor &&
                    level.get(baseX, baseY + 1) == (int32_t)Basic::floor && level.get(baseX + 1, baseY + 1) == (int32_t)Basic::floor)
                {
                    level.get(baseX, baseY) = (int32_t)Basic::upStairs;
                    return true;
                }
            }
        }
        else
        {
            for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
            {
                if (rooms[i].width >= 6 && rooms[i].height >= 6)
                {
                    int32_t baseX = rooms[i].centre().first;
                    int32_t baseY = rooms[i].centre().second;

                    if (level.get(baseX, baseY) != (int32_t)Basic::floor)
                        continue;

                    level.get(baseX, baseY) = (int32_t)Basic::upStairs;

                    return true;
                }
            }
        }

        return false;
    }

    bool placeDownStairs(Level::Dun& level, const std::vector<Room>& rooms, int32_t levelNum)
    {
        if (levelNum == 3)
        {
            for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
            {
                int32_t baseX = rooms[i].xPos;
                int32_t baseY = rooms[i].yPos + (rooms[i].width / 2);

                // on a wall
                if (level.get(baseX - 2, baseY + 1) == (int32_t)Basic::blank && level.get(baseX - 2, baseY) == (int32_t)Basic::blank &&
                    level.get(baseX - 2, baseY + 1) == (int32_t)Basic::blank && level.get(baseX - 1, baseY + 1) == (int32_t)Basic::blank &&
                    level.get(baseX - 1, baseY) == (int32_t)Basic::blank && level.get(baseX - 1, baseY + 1) == (int32_t)Basic::blank &&
                    level.get(baseX, baseY - 1) == (int32_t)Basic::wall && level.get(baseX, baseY) == (int32_t)Basic::wall &&
                    level.get(baseX, baseY + 1) == (int32_t)Basic::wall && level.get(baseX + 1, baseY - 1) == (int32_t)Basic::floor &&
                    level.get(baseX + 1, baseY) == (int32_t)Basic::floor && level.get(baseX + 1, baseY + 1) == (int32_t)Basic::floor)
                {
                    level.get(baseX, baseY) = (int32_t)Basic::downStairs;
                    return true;
                }
            }
        }
        for (int32_t i = rooms.size() - 1; i != 0; i--)
        {
            if (rooms[i].width >= 6 && rooms[i].height >= 6)
            {
                int32_t baseX = rooms[i].centre().first;
                int32_t baseY = rooms[i].centre().second;

                if (level.get(baseX, baseY) != (int32_t)Basic::floor)
                    continue;

                level.get(baseX, baseY) = (int32_t)Basic::downStairs;

                return true;
            }
        }

        return false;
    }

#define ROOMAREA 30

    // Generates a flat map (no information about wall direction, etc)
    // Uses the tinykeep level generation algorithm, described here:
    // http://www.reddit.com/r/gamedev/comments/1dlwc4/procedural_dungeon_generation_algorithm_explained/
    // The basic algorithm is as follows:
    //     1. Generate a bunch of rooms in a radius around the centre of the map, with rooms weighted
    //        towards being small more often than large.
    //     2. Use separation steering to spread them out until they no longer overlap.
    //     3. Split the rooms into two types, real rooms, and corridoor rooms, where real rooms are rooms
    //        with an area above a certain threshold, and corridoor rooms are the rest.
    //     4. Construct a minimum spanning tree which connects all the rooms together, then add in some
    //        extra edges to allow for some loops.
    //     5. Connect the rooms according to the graph from the last step with l shaped corridoors, and
    //        also draw any corridoor rooms that the corridoors overlap as part of the corridoor.
    Level::Dun generateTmp(int32_t width, int32_t height, int32_t levelNum)
    {
        Level::Dun level(width, height);

        // Initialise whole dungeon to blank
        for (int32_t x = 0; x < width; x++)
            for (int32_t y = 0; y < height; y++)
                level.get(x, y) = (int32_t)Basic::blank;

        std::vector<Room> rooms;
        std::vector<Room> corridoorRooms;
        generateRooms(rooms, width, height);

        // Split rooms into real rooms, and corridoor rooms
        for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
        {
            if (rooms[i].area() < ROOMAREA)
            {
                corridoorRooms.push_back(rooms[i]);
                rooms.erase(rooms.begin() + i);
                i--;
            }
        }

        // Create graph with edge from each room to each other room
        std::vector<std::vector<int32_t>> graph(rooms.size());
        for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
        {
            graph[i].resize(rooms.size());

            for (int32_t j = 0; j < (int32_t)rooms.size(); j++)
                graph[i][j] = rooms[i].distance(rooms[j]);
        }

        // Create Minimum spanning tree of above graph, and connect rooms according to edges
        std::vector<int32_t> parent;
        minimumSpanningTree(graph, parent);
        for (int32_t i = 1; i < (int32_t)rooms.size(); i++)
            connect(rooms[parent[i]], rooms[i], corridoorRooms, level);

        // Add in an extra 15% of the number of rooms random connections to create some loops
        int32_t fifteenPercent = static_cast<int32_t>(((rooms.size()) / 100.0) * 15.0);
        for (int32_t i = 0; i < fifteenPercent; i++)
        {
            int32_t a, b;

            do
            {
                a = Random::randomInRange(0, rooms.size() - 1);
                b = Random::randomInRange(0, rooms.size() - 1);
            } while (a == b || parent[a] == b || parent[b] == a);

            connect(rooms[a], rooms[b], corridoorRooms, level);
        }

        // Draw rooms on top of corridoors
        for (int32_t i = 0; i < (int32_t)rooms.size(); i++)
            drawRoom(rooms[i], level);

        // Bound corridoors with walls
        addWalls(level);

        cleanup(level, rooms);
        addDoors(level, rooms, levelNum);

        // Make sure we always place stairs
        if (!(placeUpStairs(level, rooms, levelNum) && placeDownStairs(level, rooms, levelNum)))
            return generateTmp(width, height, levelNum);

        // Separate internal from external walls
        for (int32_t x = 0; x < (int32_t)width; x++)
        {
            for (int32_t y = 0; y < (int32_t)height; y++)
            {
                if (level.get(x, y) == (int32_t)Basic::wall && !borders(x, y, Basic::blank, level))
                    level.get(x, y) = (int32_t)Basic::insideWall;
            }
        }

        cleanLooseWalls(level, true);

        return level;
    }

    bool isPassable(int32_t x, int32_t y, const Level::Dun& tmpLevel)
    {
        return getXY(x, y, tmpLevel) == (int32_t)Basic::floor || getXY(x, y, tmpLevel) == (int32_t)Basic::door;
    }

    void setPoint(int32_t x, int32_t y, int32_t val, const Level::Dun& tmpLevel, Level::Dun& level, int32_t wallOffset, bool isInsideWall)
    {
        int32_t newVal = val;

        if (val == (int32_t)TileSetEnum::xWall + wallOffset)
        {
            if (getXY(x, y, tmpLevel) == (int32_t)Basic::door && isInsideWall)
                newVal = (int32_t)TileSetEnum::xDoor;
            else if (getXY(x, y, tmpLevel) == (int32_t)Basic::upStairs)
                newVal = (int32_t)TileSetEnum::upStairs;
            else if (getXY(x - 1, y, tmpLevel) == (int32_t)Basic::blank && getXY(x, y + 1, tmpLevel) == (int32_t)Basic::blank)
                newVal = (int32_t)TileSetEnum::outsideLeftCorner;
            else if (!isInsideWall && getXY(x, y + 1, tmpLevel) == (int32_t)Basic::blank)
                newVal = (int32_t)TileSetEnum::outsideXWall;
            else if (getXY(x + 1, y, tmpLevel) == (int32_t)Basic::floor) // && isInsideWall)
                newVal = (int32_t)TileSetEnum::insideXWallEnd;
            else if (isWall(x + 1, y, tmpLevel, isInsideWall) && isWall(x, y - 1, tmpLevel, isInsideWall))
                newVal = (int32_t)TileSetEnum::leftCorner + wallOffset;
            else if (getXY(x - 1, y, tmpLevel) == (int32_t)Basic::floor) // && isInsideWall)
                newVal = (int32_t)TileSetEnum::insideXWallEndBack;
        }

        else if (val == (int32_t)TileSetEnum::yWall + wallOffset)
        {
            if (getXY(x, y, tmpLevel) == (int32_t)Basic::door && isInsideWall)
                newVal = (int32_t)TileSetEnum::yDoor;
            else if (!isInsideWall && getXY(x + 1, y, tmpLevel) == (int32_t)Basic::blank)
                newVal = (int32_t)TileSetEnum::outsideYWall;
            else if (getXY(x, y + 1, tmpLevel) == (int32_t)Basic::floor) // && isInsideWall)
                newVal = (int32_t)TileSetEnum::insideYWallEnd;
            else if (getXY(x, y - 1, tmpLevel) == (int32_t)Basic::floor) // && isInsideWall)
                newVal = (int32_t)TileSetEnum::insideYWallEndBack;
        }

        else if (val == (int32_t)TileSetEnum::bottomCorner + wallOffset)
        {
            if (!isInsideWall && (getXY(x + 1, y + 1, tmpLevel) == (int32_t)Basic::blank || getXY(x + 1, y, tmpLevel) == (int32_t)Basic::blank ||
                                  getXY(x, y + 1, tmpLevel) == (int32_t)Basic::blank))
            {
                if (isWall(x, y + 1, tmpLevel, isInsideWall))
                    newVal = (int32_t)TileSetEnum::outsideYWall;
                else
                    newVal = (int32_t)TileSetEnum::outsideBottomCorner;
            }
            else if (isWall(x, y + 1, tmpLevel, isInsideWall))
                newVal = (int32_t)TileSetEnum::yWall + wallOffset;
        }

        else if (!isInsideWall && val == (int32_t)TileSetEnum::topCorner)
        {
            if (getXY(x + 1, y + 1, tmpLevel) == (int32_t)Basic::blank)
                newVal = (int32_t)TileSetEnum::outsideTopCorner;
        }

        else if (!isInsideWall && val == (int32_t)TileSetEnum::rightCorner)
        {
            if (getXY(x + 1, y - 1, tmpLevel) == (int32_t)Basic::blank || getXY(x + 1, y, tmpLevel) == (int32_t)Basic::blank ||
                getXY(x, y - 1, tmpLevel) == (int32_t)Basic::blank)
                newVal = (int32_t)TileSetEnum::outsideRightCorner;
        }

        else if (!isInsideWall && val == (int32_t)TileSetEnum::leftCorner)
        {
            if (getXY(x - 1, y + 1, tmpLevel) == (int32_t)Basic::blank || getXY(x - 1, y, tmpLevel) == (int32_t)Basic::blank ||
                getXY(x, y + 1, tmpLevel) == (int32_t)Basic::blank)
                newVal = (int32_t)TileSetEnum::outsideLeftCorner;
        }

        level.get(x, y) = (int32_t)newVal;
    }

    void placeMonsters(FAWorld::GameLevel& level, const DiabloExe::DiabloExe& exe, int32_t dLvl)
    {
        std::vector<const DiabloExe::Monster*> possibleMonsters = exe.getMonstersInLevel(dLvl);

        for (int32_t i = 0; i < (level.height() + level.width()) / 2; i++)
        {
            int32_t xPos, yPos;

            do
            {
                xPos = Random::randomInRange(1, level.width() - 1);
                yPos = Random::randomInRange(1, level.height() - 1);
            } while (!level.getTile(xPos, yPos).passable() && std::make_pair(xPos, yPos) != level.upStairsPos() &&
                     std::make_pair(xPos, yPos) != level.downStairsPos());

            std::string name = possibleMonsters[Random::randomInRange(0, possibleMonsters.size() - 1)]->monsterName;
            DiabloExe::Monster monster = exe.getMonster(name);

            FAWorld::Actor* monsterObj = new FAWorld::Actor(*level.getWorld(), monster);
            monsterObj->teleport(&level, FAWorld::Position(xPos, yPos));
        }
    }

    void fillIsometric(Level::Dun& tmpLevel, Level::Dun& level, bool ignoreNotWall, int32_t wallOffset, bool isInsideWall)
    {
        for (int32_t x = 0; x < (int32_t)tmpLevel.width(); x++)
        {
            for (int32_t y = 0; y < (int32_t)tmpLevel.height(); y++)
            {
                if (tmpLevel.get(x, y) == (int32_t)Basic::upStairs)
                {
                    level.get(x, y) = (int32_t)TileSetEnum::upStairs;
                    continue;
                }
                if (tmpLevel.get(x, y) == (int32_t)Basic::downStairs)
                {
                    level.get(x, y) = (int32_t)TileSetEnum::downStairs;
                    continue;
                }

                if (isWall(x, y, tmpLevel, isInsideWall))
                {
                    if (isWall(x + 1, y, tmpLevel, isInsideWall))
                    {
                        if (isWall(x, y + 1, tmpLevel, isInsideWall))
                            setPoint(x, y, (int32_t)TileSetEnum::topCorner + wallOffset, tmpLevel, level, wallOffset, isInsideWall);
                        else
                        {
                            setPoint(x, y, (int32_t)TileSetEnum::xWall + wallOffset, tmpLevel, level, wallOffset, isInsideWall);
                        }
                    }
                    else if (isWall(x - 1, y, tmpLevel, isInsideWall))
                    {
                        if (isWall(x, y - 1, tmpLevel, isInsideWall))
                            setPoint(x, y, (int32_t)TileSetEnum::bottomCorner + wallOffset, tmpLevel, level, wallOffset, isInsideWall);
                        else
                        {
                            if (isWall(x, y + 1, tmpLevel, isInsideWall))
                                setPoint(x, y, (int32_t)TileSetEnum::rightCorner + wallOffset, tmpLevel, level, wallOffset, isInsideWall);
                            else
                                setPoint(x, y, (int32_t)TileSetEnum::xWall + wallOffset, tmpLevel, level, wallOffset, isInsideWall);
                        }
                    }
                    else
                    {
                        setPoint(x, y, (int32_t)TileSetEnum::yWall + wallOffset, tmpLevel, level, wallOffset, isInsideWall);
                    }
                }
                else if (!ignoreNotWall)
                {
                    if (tmpLevel.get(x, y) == (int32_t)Basic::blank)
                        level.get(x, y) = (int32_t)TileSetEnum::blank;
                    else if (tmpLevel.get(x, y) == (int32_t)Basic::insideWall)
                        level.get(x, y) = (int32_t)TileSetEnum::insideXWall;
                    else
                        level.get(x, y) = (int32_t)TileSetEnum::floor;
                }
            }
        }
    }

    bool connectHelperY(int32_t x, int32_t y, Level::Dun& level)
    {
        return level.get(x, y) == (int32_t)TileSetEnum::insideYWall || level.get(x, y) == (int32_t)TileSetEnum::yDoor ||
               level.get(x, y) == (int32_t)TileSetEnum::insideXWallEnd || level.get(x, y) == (int32_t)TileSetEnum::insideXWallEndBack ||
               level.get(x, y) == (int32_t)TileSetEnum::insideLeftCorner || level.get(x, y) == (int32_t)TileSetEnum::insideRightCorner ||
               level.get(x, y) == (int32_t)TileSetEnum::insideTopCorner || level.get(x, y) == (int32_t)TileSetEnum::insideBottomCorner;
    }
    bool connectHelperX(int32_t x, int32_t y, Level::Dun& level)
    {
        return level.get(x, y) == (int32_t)TileSetEnum::insideXWall || level.get(x, y) == (int32_t)TileSetEnum::xDoor ||
               level.get(x, y) == (int32_t)TileSetEnum::insideYWallEnd || level.get(x, y) == (int32_t)TileSetEnum::insideYWallEndBack ||
               level.get(x, y) == (int32_t)TileSetEnum::insideLeftCorner || level.get(x, y) == (int32_t)TileSetEnum::insideRightCorner ||
               level.get(x, y) == (int32_t)TileSetEnum::insideTopCorner || level.get(x, y) == (int32_t)TileSetEnum::insideBottomCorner;
    }

    void connectWalls(Level::Dun& level)
    {
        for (int32_t x = 0; x < (int32_t)level.width(); x++)
        {
            for (int32_t y = 0; y < (int32_t)level.height(); y++)
            {
                switch (level.get(x, y))
                {
                    case (int32_t)TileSetEnum::yWall:
                    {
                        if (connectHelperX(x + 1, y, level))
                            level.get(x, y) = (int32_t)TileSetEnum::joinY;
                        break;
                    }

                    case (int32_t)TileSetEnum::rightCorner:
                    {
                        if (connectHelperX(x + 1, y, level) && connectHelperY(x, y - 1, level))
                        {
                            level.get(x, y) = (int32_t)TileSetEnum::joinRightCorner;
                        }
                        else
                        {
                            if (connectHelperX(x + 1, y, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinYRightCorner;
                            else if (connectHelperY(x, y - 1, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinOutXRightCorner;
                        }
                        break;
                    }

                    case (int32_t)TileSetEnum::outsideXWall:
                    {
                        if (connectHelperY(x, y - 1, level))
                            level.get(x, y) = (int32_t)TileSetEnum::joinOutX;
                        break;
                    }

                    case (int32_t)TileSetEnum::outsideTopCorner:
                    {
                        if (connectHelperX(x - 1, y, level) && connectHelperY(x, y - 1, level))
                        {
                            level.get(x, y) = (int32_t)TileSetEnum::joinTopCorner;
                        }
                        else
                        {
                            if (connectHelperX(x - 1, y, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinOutYTopCorner;
                            else if (connectHelperY(x, y - 1, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinOutXTopCorner;
                        }
                        break;
                    }

                    case (int32_t)TileSetEnum::outsideYWall:
                    {
                        if (connectHelperX(x - 1, y, level))
                            level.get(x, y) = (int32_t)TileSetEnum::joinOutY;
                        break;
                    }

                    case (int32_t)TileSetEnum::leftCorner:
                    {
                        if (connectHelperX(x - 1, y, level) && connectHelperY(x, y + 1, level))
                        {
                            level.get(x, y) = (int32_t)TileSetEnum::joinLeftCorner;
                        }
                        else
                        {
                            if (connectHelperX(x - 1, y, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinOutYLeftCorner;
                            else if (connectHelperY(x, y + 1, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinXLeftCorner;
                        }
                        break;
                    }

                    case (int32_t)TileSetEnum::xWall:
                    {
                        if (connectHelperY(x, y + 1, level))
                            level.get(x, y) = (int32_t)TileSetEnum::joinX;
                        break;
                    }

                    case (int32_t)TileSetEnum::bottomCorner:
                    {
                        if (connectHelperX(x + 1, y, level) && connectHelperY(x, y + 1, level))
                        {
                            level.get(x, y) = (int32_t)TileSetEnum::joinBottomCorner;
                        }
                        else
                        {
                            if (connectHelperX(x + 1, y, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinYBottomCorner;
                            else if (connectHelperY(x, y + 1, level))
                                level.get(x, y) = (int32_t)TileSetEnum::joinXBottomCorner;
                        }
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
        }
    }

    FAWorld::GameLevel*
    generate(FAWorld::World& world, int32_t width, int32_t height, int32_t dLvl, const DiabloExe::DiabloExe& exe, int32_t previous, int32_t next)
    {
        Random::FAsrand(123);
        int32_t levelNum = ((dLvl - 1) / 4) + 1;

        Level::Dun tmpLevel = generateTmp(width, height, levelNum);

        Level::Dun level(width, height);

        std::stringstream ss;
        ss << "resources/tilesets/l" << levelNum << ".ini";
        TileSet tileset(ss.str());

        fillIsometric(tmpLevel, level, false, 0, false);
        fillIsometric(tmpLevel, level, true, (int32_t)TileSetEnum::insideXWall, true);

        connectWalls(level);

        std::pair<int32_t, int32_t> downStairsPoint;
        std::pair<int32_t, int32_t> upStairsPoint;

        for (int32_t x = 0; x < (int32_t)width; x++)
        {
            for (int32_t y = 0; y < (int32_t)height; y++)
            {
                if (level.get(x, y) == (int32_t)TileSetEnum::upStairs)
                {
                    upStairsPoint = std::make_pair(x * 2, y * 2);
                    level.get(x, y) = (int32_t)TileSetEnum::floor;
                }
                else if (level.get(x, y) == (int32_t)TileSetEnum::downStairs)
                {
                    downStairsPoint = std::make_pair(x * 2, y * 2);
                    level.get(x, y) = (int32_t)TileSetEnum::floor;
                }
                else
                    level.get(x, y) = tileset.convert((TileSetEnum)level.get(x, y));
            }
        }

        // Add in some random aesthetic variation
        for (int32_t x = 0; x < (int32_t)width; x++)
        {
            for (int32_t y = 0; y < (int32_t)height; y++)
            {
                // else
                {
                    level.get(x, y) = tileset.getRandomTile(level.get(x, y));
                }
            }
        }

        // place up stairs
        {
            int32_t x = upStairsPoint.first / 2, y = upStairsPoint.second / 2;

            level.get(x - 1, y - 1) = tileset.upStairs1;
            level.get(x, y - 1) = tileset.upStairs2;
            level.get(x + 1, y - 1) = tileset.upStairs3;

            level.get(x - 1, y) = tileset.upStairs4;
            level.get(x, y) = tileset.upStairs5;
            level.get(x + 1, y) = tileset.upStairs6;

            level.get(x - 1, y + 1) = tileset.upStairs7;
            level.get(x, y + 1) = tileset.upStairs8;
            level.get(x + 1, y + 1) = tileset.upStairs9;
        }

        // place down stairs
        {
            int32_t x = downStairsPoint.first / 2, y = downStairsPoint.second / 2;

            level.get(x - 1, y - 1) = tileset.downStairs1;
            level.get(x, y - 1) = tileset.downStairs2;
            level.get(x + 1, y - 1) = tileset.downStairs3;

            level.get(x - 1, y) = tileset.downStairs4;
            level.get(x, y) = tileset.downStairs5;
            level.get(x + 1, y) = tileset.downStairs6;

            level.get(x - 1, y + 1) = tileset.downStairs7;
            level.get(x, y + 1) = tileset.downStairs8;
            level.get(x + 1, y + 1) = tileset.downStairs9;
        }

        ss.str("");
        ss << "levels/l" << levelNum << "data/l" << levelNum << ".cel";
        std::string celPath = ss.str();

        ss.str("");
        ss << "levels/l" << levelNum << "data/l" << levelNum << ".til";
        std::string tilPath = ss.str();

        ss.str("");
        ss << "levels/l" << levelNum << "data/l" << levelNum << ".min";
        std::string minPath = ss.str();

        ss.str("");
        ss << "levels/l" << levelNum << "data/l" << levelNum << ".sol";
        std::string solPath = ss.str();

        Level::Level levelBase(std::move(level), tilPath, minPath, solPath, celPath, downStairsPoint, upStairsPoint, tileset.getDoorMap(), previous, next);
        auto retval = new FAWorld::GameLevel(world, std::move(levelBase), dLvl);

        placeMonsters(*retval, exe, dLvl);

        return retval;
    }
}
