#include "levelgen.h"

#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <iostream>

#include <cmath>

#include "random.h"
#include "mst.h"

namespace Freeablo
{
    class Room
    {
        public:
            size_t xPos;
            size_t yPos;
            size_t width;
            size_t height;

            Room(size_t _xPos, size_t _yPos, size_t _width, size_t _height): xPos(_xPos), yPos(_yPos), width(_width), height(_height) {}

            bool intersects(const Room& other) const
            {
                return !(yPos+height <= other.yPos+1 || yPos >= other.yPos+other.height-1 || xPos+width <= other.xPos+1 || xPos >= other.xPos+other.width-1);
            }

            std::pair<int32_t, int32_t> centre() const
            {
                return std::pair<int32_t, int32_t>(xPos + (width/2), yPos + (height/2));
            }

            size_t area() const
            {
                return width*height;
            }

            size_t distance(const Room& other) const
            {
                return sqrt((centre().first - other.centre().first)*(centre().first - other.centre().first) + (centre().second - other.centre().second)*(centre().second - other.centre().second));
            }
    };

    enum DunTile
    {
        dunXWall = 2,
        dunOutsideXWall = 19,
        dunYWall = 1,
        dunOutsideYWall = 18,
        dunBottomCorner = 3,
        dunOutsideBottomCorner = 20,
        dunRightCorner = 6,
        dunOutsideRightCorner = 23,
        dunLeftCorner = 7,
        dunOutsideLeftCorner = 24,
        dunTopCorner = 4,
        dunOutsideTopCorner = 21,
        dunFloor = 13,
        dunBlank = 22,
        dunXWallEnd = 17,
        dunYWallEnd = 16
    };

    enum Basic
    {
        wall = 64,
        door = 72,
        floor = 13,
        blank = 104,
    };

    void fillRoom(const Room& room, DunFile& level)
    {
        for(size_t x = 0; x < room.width; x++)
        {
            for(size_t y = 0; y < room.height; y++)
            {
                level.at(x + room.xPos, y + room.yPos) = floor;
            }
        }
    }

    void drawCorridoorSegment(const Room& room, const std::vector<Room>& corridoorRooms, DunFile& level)
    {
        fillRoom(room, level);

        for(size_t i = 0; i < corridoorRooms.size(); i++)
        {
            if(room.intersects(corridoorRooms[i]))
                fillRoom(corridoorRooms[i], level);
        }
    }
    
    // Ensures that two points are connected by inserting an l-shaped corridoor,
    // also draws any rooms in rooms vector that the corridoor intersects
    void connect(const Room& a, const Room& b, const std::vector<Room>& rooms, DunFile& level)
    {
        size_t ax = a.centre().first;
        size_t ay = a.centre().second;

        size_t bx = b.centre().first;
        size_t by = b.centre().second;
        
        if(bx != ax)
        {
            if(bx > ax)
            {
                Room x(ax, ay-1, bx-ax, 3);
                drawCorridoorSegment(x, rooms, level);
            }
            else
            {
                Room x(bx, ay-1, ax-bx, 3); 
                drawCorridoorSegment(x, rooms, level);
            }
        }
        if(by != ay)
        {
            if(by > ay)
            {
                Room y(bx-1, ay, 3, by-ay);
                drawCorridoorSegment(y, rooms, level);
            }
            else
            {
                Room y(bx-1, by, 3, ay-by+2);
                drawCorridoorSegment(y, rooms, level);
                
            }

        }
    }
   
    // Move room in direction specified by normalised vector, making sure to keep within
    // grid of size width * height
    void moveRoom(Room& room, const std::pair<float, float>& vector, size_t width, size_t height)
    {
        if(vector.first == 0 && vector.second == 0)
            return;
        
        float angle = (std::atan2(vector.second, vector.first) / M_PI) * 180.0;

        if(angle < 0)
            angle = 360 + angle;
        
        int32_t xMove;
        int32_t yMove;

        if((angle <= 22.5 && angle >= 0.0) || (angle <= 360.0 && angle >= 337.5))
        {
            xMove = 1;
            yMove = 0;
        }
        else if(angle <= 67.5 && angle >= 22.5)
        {
            xMove = 1;
            yMove = 1;
        }
        else if(angle <= 112.5 && angle >= 67.5)
        {
            xMove = 0;
            yMove = 1;
        }
        else if(angle <= 157.5 && angle >= 112.5)
        {
            xMove = -1;
            yMove = 1;
        }
        else if(angle <= 202.5 && angle >= 157.5)
        {
            xMove = -1;
            yMove = 0;
        }
        else if(angle <= 247.5 && angle >= 202.5)
        {
            xMove = -1;
            yMove = -1;
        }
        else if(angle <= 292.5 && angle >= 247.5)
        {
            xMove = 0;
            yMove = -1;
        }
        else if(angle <= 337.5 && angle >= 292.5)
        {
            xMove = 1;
            yMove = -1;
        }

        int32_t newX = room.xPos + xMove;
        int32_t newY = room.yPos + yMove;
        
        // Make sure not to move outside map
        if(newX >= 0 && newY >= 0 && newX+room.width < width && newY+room.height < height)
        {
            room.xPos = newX;
            room.yPos = newY;
        }
    }

    void normalise(std::pair<float, float>& vector)
    {
        float magnitude = sqrt(vector.first*vector.first + vector.second*vector.second);
        vector.first /= (float)magnitude;
        vector.second /= (float)magnitude;
    }
    
    // Removes the room overlapping the largest number of rooms repeatedly,
    // until there are no overlaps
    void removeOverlaps(std::vector<Room>& rooms)
    {
        bool overlap = true;

        while(overlap)
        {
            overlap = false;

            int32_t maxIndex = -1;
            size_t maxNeighbourCount = 0;

            for(int i = 0; i < rooms.size(); i++)
            {
                size_t neighbourCount = 0;

                for(int j = 0; j < rooms.size(); j++)
                {
                    if(i != j && rooms[i].intersects(rooms[j]))
                    {
                        neighbourCount++;
                        overlap = true;
                    }
                }
                
                if(neighbourCount > maxNeighbourCount)
                {
                    maxIndex = i;
                    maxNeighbourCount = neighbourCount;
                }
            }

            if(maxIndex > -1)
                rooms.erase(rooms.begin() + maxIndex);
        }
    }
    
    // Separate rooms so they don't overlap, using flocking ai
    // based on the algorithm described here: 
    // http://gamedevelopment.tutsplus.com/tutorials/the-three-simple-rules-of-flocking-behaviors-alignment-cohesion-and-separation--gamedev-3444
    void separate(std::vector<Room>& rooms, size_t width, size_t height)
    {
        bool overlap = true;

        int its = 0;

        while(its < 400 && overlap)
        {
            its++;

            overlap = false;

            for(size_t i = 0; i < rooms.size(); i++)
            {
                std::pair<float, float> vector(0, 0);

                std::pair<int32_t, int32_t> currentCentre = rooms[i].centre();
                
                size_t neighbourCount = 0;

                for(size_t j = 0; j < rooms.size(); j++)
                {
                    if(i == j)
                        continue;
                    
                    bool intersects = rooms[i].intersects(rooms[j]);

                    overlap = overlap || intersects;

                    if(intersects)
                    {
                        std::pair<int32_t, int32_t> centre = rooms[j].centre();

                        if(centre.first == currentCentre.first && centre.second == currentCentre.second)
                        {
                            vector.first = randomInRange(0, 10);
                            vector.second = randomInRange(0, 10);
                            neighbourCount++;
                            continue;
                        }
                        
                        std::pair<float, float> iToJ (centre.first - currentCentre.first, centre.second - currentCentre.second);
                        normalise(iToJ);

                        vector.first += iToJ.first * rooms[i].distance(rooms[j]);
                        vector.second += iToJ.second * rooms[i].distance(rooms[j]);

                        neighbourCount++;
                    }
                }

                if(vector.first == 0 && vector.second == 0)
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
        
        if(overlap)
            removeOverlaps(rooms);    
    }

    void generateRooms(std::vector<Room>& rooms, size_t width, size_t height)
    {
        size_t maxDimension = 10;

        size_t placed = 0;

        int rSquared = 15*15;

        int centreX = width/2;
        int centreY = height/2;

        while(placed < 150)
        {
            Room newRoom(randomInRange(0, width-4), randomInRange(0, height-4), 0, 0);

            if(((centreX-newRoom.centre().first)*(centreX-newRoom.centre().first) + (centreY-newRoom.centre().second)*(centreY-newRoom.centre().second)) > rSquared)
                continue;
            
            newRoom.width = normRand(4, std::min(width-newRoom.xPos, maxDimension));
            newRoom.height = normRand(4, std::min(height-newRoom.yPos, maxDimension));
            
            float ratio = ((float)newRoom.width) / ((float)newRoom.height);

            if(ratio < 0.5 || ratio > 2.0)
                continue;

            placed++;
            rooms.push_back(newRoom);
        }

        separate(rooms, width, height);
    }

    void drawRoom(const Room& room, DunFile& level)
    {
        // Draw x oriented walls
        for(size_t x = 0; x < room.width; x++)
        {
            level.at(x + room.xPos, room.yPos) = wall;
            level.at(x + room.xPos, room.height-1 + room.yPos) = wall;
        }

        // Draw y oriented walls
        for(size_t y = 0; y < room.height; y++)
        {
            level.at(room.xPos, y + room.yPos) = wall;
            level.at(room.width-1 + room.xPos, y + room.yPos) = wall;
        }
        
        // Fill ground
        for(size_t x = 1; x < room.width-1; x++)
        {
            for(size_t y = 1; y < room.height-1; y++)
            {
                level.at(x + room.xPos, y + room.yPos) = floor;
            }
        }
    }
    
    // Get the value at (x,y) in level, or zero if it is an invalid position
    size_t getXY(int32_t x, int32_t y, const DunFile& level)
    {
        if(x < 0 || x >= level.mWidth || y < 0 || y >= level.mHeight)
            return 0;
        
        return level.at(x, y);
    }
    
    // Returns true if the tile at (x,y) in level borders any tile of the value tile,
    // false otherwise
    bool borders(size_t x, size_t y, Basic tile, const DunFile& level)
    {
        for(int32_t xoffs = -1; xoffs < 2; xoffs++)
        {
            for(int32_t yoffs = -1; yoffs < 2; yoffs++)
            {
                int32_t testX = xoffs + x;
                int32_t testY = yoffs + y;
                
                if(getXY(testX, testY, level) == tile)
                    return true;
            }
        }
        
        return false;
    }
    
    // Remove double walls, as the tileset does not allow for them
    void cleanup(DunFile& level)
    {
        std::vector<std::pair<size_t, size_t> > toFix;

        for(int32_t x = 0; x < level.mWidth; x++)
        {
            for(int32_t y = 0; y < level.mHeight; y++)
            {
                if(level.at(x, y) != wall || borders(x, y, blank, level))
                    continue;
                
                // This expression is a bit of a monster, take my word for it that it finds double walls :P
                if((level.at(x, y-1) == wall && (((level.at(x+1, y-1) == wall && level.at(x+1, y) == wall)) ||
                                                  (level.at(x-1, y-1) == wall && level.at(x-1, y) == wall))   ) 
                    ||
                   (level.at(x, y+1) == wall && (((level.at(x+1, y+1) == wall && level.at(x+1, y) == wall)) ||
                                                  (level.at(x-1, y+1) == wall && level.at(x-1, y) == wall))   ))
                {
                    toFix.push_back(std::pair<size_t, size_t>(x, y));
                }
            }
        }

        for(size_t i = 0; i < toFix.size(); i++)
            level.at(toFix[i].first, toFix[i].second) = floor;
        
        // Remove any isolated wall blocks which may have been created by removing double walls 
        for(int32_t x = 0; x < level.mWidth; x++)
            for(int32_t y = 0; y < level.mHeight; y++)
                if(level.at(x, y) == wall && getXY(x+1, y, level) != wall && getXY(x-1, y, level) != wall &&
                                             getXY(x, y+1, level) != wall && getXY(x, y-1, level) != wall)
                    level.at(x, y) = floor;
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
    void generateTmp(size_t width, size_t height, DunFile& level)
    {
        level.resize(width, height);
        
        // Initialise whole dungeon to blank
        for(size_t x = 0; x < width; x++)
            for(size_t y = 0; y < height; y++)
                level.at(x, y) = blank;

        std::vector<Room> rooms;
        std::vector<Room> corridoorRooms;
        generateRooms(rooms, width, height);
        
        // Split rooms into real rooms, and corridoor rooms
        for(size_t i = 0; i < rooms.size(); i++)
        {
            if(rooms[i].area() < ROOMAREA)
            {
                corridoorRooms.push_back(rooms[i]);
                rooms.erase(rooms.begin()+i);
                i--;
            }
        }
        
        // Create graph with edge from each room to each other room
        std::vector<std::vector<size_t> > graph(rooms.size());
        for(size_t i = 0; i < rooms.size(); i++)
        {
            graph[i].resize(rooms.size());

            for(size_t j = 0; j < rooms.size(); j++)
                graph[i][j] = rooms[i].distance(rooms[j]);
        }

        // Create Minimum spanning tree of above graph, and connect rooms according to edges
        std::vector<size_t> parent;
        minimumSpanningTree(graph, parent);
        for(size_t i = 1; i < rooms.size(); i++)
            connect(rooms[parent[i]], rooms[i], corridoorRooms, level);
        
        // Add in an extra 15% of the number of rooms random connections to create some loops
        int fifteenPercent = (((float)rooms.size())/100.0)*15.0;
        for(size_t i = 0; i < fifteenPercent; i++)
        {
            size_t a, b;
            
            do
            {
                a = randomInRange(0, rooms.size()-1);
                b = randomInRange(0, rooms.size()-1);
            }
            while(a == b || parent[a] == b || parent[b] == a);
            
            connect(rooms[a], rooms[b], corridoorRooms, level);
        }

        // Draw rooms on top of corridoors
        for(int i = 0; i < rooms.size(); i++)
            drawRoom(rooms[i], level);
        
        // Bound corridoors with walls
        for(size_t x = 0; x < width; x++)
        {
            for(size_t y = 0; y < height; y++)
            {
                if(getXY(x, y, level) == blank)
                {
                    if(borders(x, y, floor, level))
                        level.at(x, y) = wall;
                }
            }
        }
        
        cleanup(level); 
    }

    void setPoint(int32_t x, int32_t y, int val, const DunFile& tmpLevel,  DunFile& level)
    {
        int newVal = val;
        switch(val)
        {
            case dunXWall:
            {   
                if(getXY(x, y+1, tmpLevel) == blank)
                    newVal = dunOutsideXWall;
                else if(getXY(x+1, y, tmpLevel) == floor)
                    newVal = dunXWallEnd;
                else if(getXY(x-1, y, tmpLevel) == floor)
                    newVal = dunLeftCorner;
                break;
            }

            case dunYWall:
            {
                if(getXY(x+1, y, tmpLevel) == blank)
                    newVal = dunOutsideYWall;
                else if(getXY(x, y+1, tmpLevel) == floor)
                    newVal = dunYWallEnd;
                else if(getXY(x, y-1, tmpLevel) == floor)
                    newVal = dunRightCorner;
                break;
            }

            case dunBottomCorner:
            {
                if(getXY(x+1, y+1, tmpLevel) == blank || getXY(x+1, y, tmpLevel) == blank || getXY(x, y+1, tmpLevel) == blank)
                {
                    if(getXY(x, y+1, tmpLevel) == wall)
                        newVal = dunOutsideYWall;
                    else
                        newVal = dunOutsideBottomCorner;
                }
                else if(getXY(x, y+1, tmpLevel) == wall)
                    newVal = dunYWall;
                break;
            }

            case dunTopCorner:
            {
                if(getXY(x+1, y+1, tmpLevel) == blank)
                    newVal = dunOutsideTopCorner;
                break;
            }

            case dunRightCorner:
            {
                if(getXY(x+1, y-1, tmpLevel) == blank  || getXY(x+1, y, tmpLevel) == blank || getXY(x, y-1, tmpLevel) == blank)
                    newVal = dunOutsideRightCorner;
                break;
            }

            case dunLeftCorner:
            {
                if(getXY(x-1, y+1, tmpLevel) == blank  || getXY(x-1, y, tmpLevel) == blank || getXY(x, y+1, tmpLevel) == blank)
                    newVal = dunOutsideLeftCorner;
                break;
            }

            default: {}
        }

        level.at(x, y) = newVal;
    }
    
    void generate(size_t width, size_t height, DunFile& level)
    {
        DunFile tmpLevel;
        generateTmp(width, height, tmpLevel);

        level.resize(width, height);
        
        // Fill in isometric information (wall direction, etc), using flat tmpLevel as a base
        for(int32_t x = 0; x < width; x++)
        {
            for(int32_t y = 0; y < height; y++)
            {
                if(tmpLevel.at(x, y) == wall)
                {
                    if(getXY(x+1, y, tmpLevel) == wall)
                    {
                        if(getXY(x, y+1, tmpLevel) == wall)
                            setPoint(x, y, dunTopCorner, tmpLevel, level);
                        else
                        {
                            if(getXY(x, y-1, tmpLevel) == wall)
                                setPoint(x, y, dunLeftCorner, tmpLevel, level);
                            else
                                setPoint(x, y, dunXWall, tmpLevel, level);
                        }
                    }
                    else if(getXY(x-1, y, tmpLevel) == wall)
                    {
                        if(getXY(x, y-1, tmpLevel) == wall)
                            setPoint(x, y, dunBottomCorner, tmpLevel, level);
                        else
                        {
                            if(getXY(x, y+1, tmpLevel) == wall)
                                setPoint(x, y, dunRightCorner, tmpLevel, level);
                            else
                                setPoint(x, y, dunXWall, tmpLevel, level);
                        }
                    }
                    else
                    {
                        setPoint(x, y, dunYWall, tmpLevel, level);
                    }
                }
                else
                {
                    if(tmpLevel.at(x, y) == blank)
                        level.at(x, y) = dunBlank;
                    else
                        level.at(x, y) = dunFloor;
                }
            }
        }
    }
}
