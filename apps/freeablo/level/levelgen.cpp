#include "levelgen.h"

#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <iostream>


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

            bool intersects(const Room& other)
            {
                return !(xPos > other.xPos + other.width || xPos + width < other.xPos || yPos > other.yPos + other.height || yPos + height < other.yPos);
            }

            size_t area()
            {
                return width*height;
            }
    };

    /*enum WallType
    {
        xWall = 2,
        yWall = 1,
        bottomCorner = 3,
        rightCorner = 7,
        leftCorner = 6,
        topCorner = 4,
        floor = 13
    };*/

    enum Basic
    {
        wall = 64,
        door = 72,
        floor = 13,
        blank = 104
    };

    void drawRoom(size_t xPos, size_t yPos, size_t width, size_t height, DunFile& level)
    {
        // Draw x oriented walls
        for(size_t x = 0; x < width; x++)
        {
            level.at(x + xPos, yPos) = wall;
            level.at(x + xPos, height-1 + yPos) = wall;
        }

        // Draw y oriented walls
        for(size_t y = 0; y < height; y++)
        {
            level.at(xPos, y + yPos) = wall;
            level.at(width-1 + xPos, y + yPos) = wall;
        }
        
        // Fill ground
        for(size_t x = 1; x  < width-1; x++)
        {
            for(size_t y = 1; y < height-1; y++)
            {
                level.at(x + xPos, y + yPos) = floor;
            }
        }
    }
    
    // Deletes unnecessary wall parts (any that do not border the outside)
    void cleanup(DunFile& level)
    {
        for(size_t x = 0; x < level.mWidth; x++)
        {
            for(size_t y = 0; y < level.mHeight; y++)
            {
                if(level.at(x, y) != wall)
                    continue;

                bool border = false;

                for(int32_t xoffs = -1; xoffs < 2; xoffs++)
                {
                    for(int32_t yoffs = -1; yoffs < 2; yoffs++)
                    {
                        int32_t testX = xoffs + x;
                        int32_t testY = yoffs + y;
                        
                        if(testX < 0 || testX >= level.mWidth || testY < 0 || testY >= level.mHeight || level.at(testX, testY) == blank)
                        {
                            border = true;
                            goto done;
                        }
                    }
                }
                done:
                
                if(!border)
                    level.at(x, y) = floor;
            }
        }
    }



    // Taken from http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
    // Would like a semi-open interval [min, max)
    int randomInRange (unsigned int min, unsigned int max)
    {
        int base_random = rand(); // in [0, RAND_MAX]
        if (RAND_MAX == base_random) 
            return randomInRange(min, max);
        // now guaranteed to be in [0, RAND_MAX)
        int range       = max - min,
            remainder   = RAND_MAX % range,
            bucket      = RAND_MAX / range;

        // There are range buckets, plus one smaller interval within remainder of RAND_MAX 
        if (base_random < RAND_MAX - remainder)
            return min + base_random/bucket;
        else
            return randomInRange (min, max);
    }

    
    // Ensures that two points are connected by inserting an l-shaped corridoor
    // TODO: Tidy
    void connect(size_t ax, size_t ay, size_t bx, size_t by, DunFile& level)
    {
        size_t x = ax, y = ay;

        bool inside = true;

        bool onX = true;

        while(x != bx || y != by)
        {
            if(onX)
            {
                if(x > bx)
                    x--;
                else if(x < bx)
                    x++;

                if(x == bx)
                    onX = false;
            }
            else
            {
                if(y > by)
                    y--;
                else
                    y++;
            }

            if(level[x][y] != blank)
                inside = true;
            else
                inside = false; 
            
            if(level[x][y] == wall)
            {
                for(int32_t xoffs = -1; xoffs < 2; xoffs++)
                {
                    for(int32_t yoffs = -1; yoffs < 2; yoffs++)
                    {
                        int32_t testX = xoffs + x;
                        int32_t testY = yoffs + y;
                        
                        if(testX < 0 || testX >= level.mWidth || testY < 0 || testY >= level.mHeight)
                            continue;
                        
                        if(level.at(testX, testY) == blank)
                            level.at(testX, testY) = wall;
                         
                    }
                }
            }

            level.at(x, y) = floor;
            
            if(!inside)
            {
                if(onX)
                {
                    level.at(x, y+1) = wall;
                    level.at(x, y-1) = wall;
                }
            }

        }
    }

    void generate(size_t width, size_t height, DunFile& level)
    {

        level.resize(width, height);
        
        // Initialise whole dungeon to ground
        for(size_t x = 0; x < width; x++)
            for(size_t y = 0; y < height; y++)
                level.at(x, y) = blank;

        srand(50); // TODO: Fix constant

        std::vector<Room> rooms;
        size_t total_area = 0;
        size_t its = 0;

        size_t maxDimension = 20;

        // Fill dungeon with disconnected rooms, attempting to fill half the total area
        while(total_area < (width*height)/2 && its < 20)
        {
            its++;

            Room newRoom(randomInRange(0, width-4), randomInRange(0, height-4), 0, 0);
            
            newRoom.width = randomInRange(4, std::min(width-newRoom.xPos, maxDimension));
            newRoom.height = randomInRange(4, std::min(height-newRoom.yPos, maxDimension));
            
            float ratio = ((float)newRoom.width) / ((float)newRoom.height);

            if(ratio < 0.5 || ratio > 2.0)
                continue;

            bool intersects = false;

            for(size_t i = 0; i < rooms.size(); i++)
                intersects = intersects || newRoom.intersects(rooms[i]);

            if(!intersects)
            {
                rooms.push_back(newRoom);
                drawRoom(newRoom.xPos, newRoom.yPos, newRoom.width, newRoom.height, level);
                total_area += newRoom.area();
                its = 0;
            }
        }
        
        std::vector<Room> connected;
        connected.push_back(rooms[1]);
        rooms.erase(rooms.begin() + 1);

        while(!rooms.empty())
        {
            int a = 0, b = randomInRange(0, connected.size());

            size_t ax = rooms[a].xPos + (rooms[a].width/2);
            size_t ay = rooms[a].yPos + (rooms[a].height/2);
            
            size_t bx = connected[b].xPos + (connected[b].width/2);
            size_t by = connected[b].yPos + (connected[b].height/2);
            
            connect(ax, ay, bx, by, level);

            rooms.erase(rooms.begin());
        }

        cleanup(level);
    }
}
