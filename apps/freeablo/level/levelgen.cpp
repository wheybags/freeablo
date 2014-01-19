#include "levelgen.h"

#include <stdlib.h>

#include <vector>
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

    void drawRoom(size_t xPos, size_t yPos, size_t width, size_t height, DunFile& level)
    {
        // Draw x oriented walls
        for(size_t x = 1; x < width-1; x++)
        {
            level.at(x + xPos, yPos) = 2;
            level.at(x + xPos, height-1 + yPos) = 2;
        }

        // Draw y oriented walls
        for(size_t y = 1; y < height-1; y++)
        {
            level.at(xPos, y + yPos) = 1;
            level.at(width-1 + xPos, y + yPos) = 1;
        }
        
        // Draw 4 corners
        level.at(width-1 + xPos, height-1 + yPos) = 3;
        level.at(xPos, height-1 + yPos) = 7;
        level.at(width-1 + xPos, yPos) = 6;
        level.at(xPos, yPos) = 4;

        // Fill ground
        for(size_t x = 1; x  < width-1; x++)
        {
            for(size_t y = 1; y < height-1; y++)
            {
                level.at(x + xPos, y + yPos) = 13;
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

    void generate(size_t width, size_t height, DunFile& level)
    {
        level.resize(width, height);
        
        // Initialise whole dungeon to ground
        for(size_t x = 0; x < width; x++)
            for(size_t y = 0; y < height; y++)
                level.at(x, y) = 13;

        srand(501); // TODO: Fix constant

        std::vector<Room> rooms;
        size_t total_area = 0;
        size_t its = 0;

        // Fill dungeon with disconnected rooms, attempting to fill half the total area
        while(total_area < (width*height)/2 && its < 100)
        {
            its++;

            Room newRoom(randomInRange(0, width-3), randomInRange(0, height-3), 0, 0);
            
            newRoom.width = randomInRange(2, width-newRoom.xPos);
            newRoom.height = randomInRange(2, height-newRoom.yPos);

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
    }

}
