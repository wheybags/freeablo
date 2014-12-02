#include "levelgen.h"

#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <sstream>

#include <cmath>

#include "random.h"
#include "mst.h"
#include "tileset.h"

#include <diabloexe/diabloexe.h>

#include <misc/misc.h>

namespace FALevelGen
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

            bool onBorder(size_t xCoord, size_t yCoord)
            {
                // Draw x oriented walls
                for(size_t x = 0; x < width; x++)
                {
                    if((xCoord == x + xPos && yCoord == yPos) ||
                       (xCoord == x + xPos && yCoord == height-1 + yPos))
                        return true; 
                    //level[x + room.xPos][room.yPos] = wall;
                    //level[x + room.xPos][room.height-1 + room.yPos] = wall;
                }

                // Draw y oriented walls
                for(size_t y = 0; y < height; y++)
                {
                    if((xCoord == xPos && yCoord == y + yPos) ||
                       (xCoord == width-1 + xPos && yCoord == y + yPos))
                        return true; 


                    //level[room.xPos][y + room.yPos] = wall;
                    //level[room.width-1 + room.xPos][y + room.yPos] = wall;
                }

                return false;
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
                return sqrt((float)((centre().first - other.centre().first)*(centre().first - other.centre().first) + (centre().second - other.centre().second)*(centre().second - other.centre().second)));
            }
    };

    enum Basic
    {
        wall = 135,
        upStairs = 64,
        downStairs = 59,
        door = 47,
        floor = 13,
        blank = 104
    };

    void fillRoom(const Room& room, Level::Dun& level)
    {
        for(size_t x = 0; x < room.width; x++)
        {
            for(size_t y = 0; y < room.height; y++)
            {
                level[x + room.xPos][y + room.yPos] = floor;
            }
        }
    }

    void drawCorridoorSegment(const Room& room, const std::vector<Room>& corridoorRooms, Level::Dun& level)
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
    void connect(const Room& a, const Room& b, const std::vector<Room>& rooms, Level::Dun& level)
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
        int32_t xMove, yMove;

        switch(Misc::getVecDir(vector))
        {
            case -1:
            {
                xMove = 0;
                yMove = 0;
                break;
            }
            case 0:
            {
                xMove = 1;
                yMove = 1;
                break;
            }
            case 1:
            {
                xMove = 0;
                yMove = 1;
                break;
            }
            case 2:
            {
                xMove = -1;
                yMove = 1;
                break;
            }
            case 3:
            {
                xMove = -1;
                yMove = 0;
                break;
            }
            case 4:
            {
                xMove = -1;
                yMove = -1;
                break;
            }
            case 5:
            {
                xMove = 0;
                yMove = -1;
                break;
            }
            case 6:
            {
                xMove = 1;
                yMove = -1;
                break;
            }
            case 7:
            {
                xMove = 1;
                yMove = 0;
                break;
            }
            default:
            {
                assert(false); // This should never happen
            }
        }
        
        int32_t newX = room.xPos + xMove;
        int32_t newY = room.yPos + yMove;

        // Make sure not to move outside map
        if(newX >= 1 && newY >= 1 && newX+room.width < width-1 && newY+room.height < height-1)
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

            for(size_t i = 0; i < rooms.size(); i++)
            {
                size_t neighbourCount = 0;

                for(size_t j = 0; j < rooms.size(); j++)
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
                        
                        std::pair<float, float> iToJ = Misc::getVec(currentCentre, centre);
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

        size_t centreX = width/2;
        size_t centreY = height/2;
       
        // The following two are based on the fact that 150 rooms in a radius of
        // 15 looks good on an 85*75 map 
        size_t numRooms = std::min(width, height)*(150.0/75.0);
        size_t radius = std::min(width, height)*(15.0/75.0);

        while(placed < numRooms)
        {
            Room newRoom(randomInRange(0, width-4), randomInRange(0, height-4), 0, 0);

            if(((centreX-newRoom.centre().first)*(centreX-newRoom.centre().first) + (centreY-newRoom.centre().second)*(centreY-newRoom.centre().second)) > radius*radius)
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

    void drawRoom(const Room& room, Level::Dun& level)
    {
        // Draw x oriented walls
        for(size_t x = 0; x < room.width; x++)
        {
            level[x + room.xPos][room.yPos] = wall;
            level[x + room.xPos][room.height-1 + room.yPos] = wall;
        }

        // Draw y oriented walls
        for(size_t y = 0; y < room.height; y++)
        {
            level[room.xPos][y + room.yPos] = wall;
            level[room.width-1 + room.xPos][y + room.yPos] = wall;
        }
        
        // Fill ground
        for(size_t x = 1; x < room.width-1; x++)
        {
            for(size_t y = 1; y < room.height-1; y++)
            {
                level[x + room.xPos][y + room.yPos] = floor;
            }
        }
    }
    
    // Get the value at (x,y) in level, or zero if it is an invalid position
    size_t getXY(int32_t x, int32_t y, const Level::Dun& level)
    {
        if(x < 0 || x >= (int32_t)level.width() || y < 0 || y >= (int32_t)level.height())
            return 0;
        
        return level[x][y];
    }
    
    // Returns true if the tile at (x,y) in level borders any tile of the value tile,
    // false otherwise
    bool borders(size_t x, size_t y, Basic tile, const Level::Dun& level)
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
    void cleanup(Level::Dun& level, std::vector<Room>& rooms)
    {
        for(int32_t x = 0; x < (int32_t)level.width(); x++)
        {
            for(int32_t y = 0; y < (int32_t)level.height(); y++)
            {
                if(level[x][y] != wall || borders(x, y, blank, level))
                    continue;
                
                // This expression is a bit of a monster, take my word for it that it finds double walls :P
                if((level[x][y-1] == wall && (((level[x+1][y-1] == wall && level[x+1][y] == wall)) ||
                                                  (level[x-1][y-1] == wall && level[x-1][y] == wall))   ) 
                    ||
                   (level[x][y+1] == wall && (((level[x+1][y+1] == wall && level[x+1][y] == wall)) ||
                                                  (level[x-1][y+1] == wall && level[x-1][y] == wall))   ))
                {
                    level[x][y] = floor;

                    for(size_t i = 0; i < rooms.size(); i++)
                    {
                        if(rooms[i].onBorder(x,y))
                        {
                            // Draw x oriented walls
                            for(size_t x = 0; x < rooms[i].width; x++)
                            {
                                if(!borders(x + rooms[i].xPos, rooms[i].yPos, blank, level))
                                    level[x + rooms[i].xPos][rooms[i].yPos] = floor;
                                if(!borders(x + rooms[i].xPos, rooms[i].height-1 + rooms[i].yPos, blank, level))
                                    level[x + rooms[i].xPos][rooms[i].height-1 + rooms[i].yPos] = floor;
                            }

                            // Draw y oriented walls
                            for(size_t y = 0; y < rooms[i].height; y++)
                            {
                                if(!borders(rooms[i].xPos, y + rooms[i].yPos, blank, level))
                                    level[rooms[i].xPos][y + rooms[i].yPos] = floor;
                                if(!borders(rooms[i].width-1 + rooms[i].xPos, y + rooms[i].yPos, blank, level))
                                    level[rooms[i].width-1 + rooms[i].xPos][y + rooms[i].yPos] = floor;
                            }

                            rooms.erase(rooms.begin() + i);
                        }
                    }
                }
            }
        }

        // Remove any isolated wall blocks which may have been created by removing double walls 
        for(int32_t x = 0; x < (int32_t)level.width(); x++)
            for(int32_t y = 0; y < (int32_t)level.height(); y++)
                if(level[x][y] == wall && getXY(x+1, y, level) != wall && getXY(x-1, y, level) != wall &&
                                             getXY(x, y+1, level) != wall && getXY(x, y-1, level) != wall)
                    level[x][y] = floor;
    }
    
    // Helper function for adding doors
    // Iterates over all blocks on a wall, and adds doors where necessary, looking at what is in the direction
    // indicated by add (1 or -1) to determine if a door is needed
    void doorAddHelper(Level::Dun& level, int32_t otherCoord, int32_t add, size_t start, size_t end, bool xAxis)
    {
        std::vector<std::pair<size_t, size_t> > region;
        bool connected = false;
        bool hole = false;

        for(size_t i = start; i < end; i++)
        {
            if((xAxis && (getXY(i, otherCoord, level) == floor || getXY(i, otherCoord, level) == door)) || 
              (!xAxis && (getXY(otherCoord, i, level) == floor || getXY(otherCoord, i, level) == door)))
            {
                hole = true;
            }
            else if((xAxis && getXY(i, otherCoord+add, level) != floor) ||
                   (!xAxis && getXY(otherCoord+add, i, level) != floor)) 
            {
                if(hole)
                    region.resize(0);
                hole = false;
            }

            if(!hole)
            {
                if((xAxis && getXY(i, otherCoord+add, level) == floor) ||
                  (!xAxis && getXY(otherCoord+add, i, level) == floor))
                {
                    if(!connected)
                    {
                        if(region.size() > 0)
                            level[region[region.size()/2].first][region[region.size()/2].second] = door;
                        
                        region.resize(0);
                        connected = true;
                    }
                    
                    if(xAxis) 
                    {
                        if(getXY(i-1, otherCoord, level) == wall && getXY(i+1, otherCoord, level) == wall)
                            region.push_back(std::pair<size_t, size_t>(i, otherCoord));
                    }
                    else
                    {
                        if(getXY(otherCoord, i-1, level) == wall && getXY(otherCoord, i+1, level) == wall)
                            region.push_back(std::pair<size_t, size_t>(otherCoord, i));
                    }
                }
                else
                {
                    connected = false;
                }
            }
        }

        if(!hole && region.size() > 0)
            level[region[region.size()/2].first][region[region.size()/2].second] = door;
    }
    
    void addDoors(Level::Dun& level, const std::vector<Room>& rooms)
    {
        for(size_t i = 0; i < rooms.size(); i++)
        {
            // Top x wall
            doorAddHelper(level, rooms[i].yPos,                   -1, rooms[i].xPos+1, rooms[i].xPos + rooms[i].width -1,  true); 
            // Bottom x wall
            doorAddHelper(level, rooms[i].yPos+rooms[i].height-1, +1, rooms[i].xPos+1, rooms[i].xPos + rooms[i].width -1,  true); 
            
            // Left y wall
            doorAddHelper(level, rooms[i].xPos,                   -1, rooms[i].yPos+1, rooms[i].yPos + rooms[i].height -1, false); 
            // Right y wall
            doorAddHelper(level, rooms[i].xPos+rooms[i].width-1,  +1, rooms[i].yPos+1, rooms[i].yPos + rooms[i].height -1, false); 
        }
    }

    bool placeUpStairs(Level::Dun& level, const std::vector<Room>& rooms, size_t levelNum)
    {
        if(levelNum == 1)
        {
            for(size_t i = 0; i < rooms.size(); i++)
            {
                size_t baseX = rooms[i].xPos + (rooms[i].width/2);
                size_t baseY = rooms[i].yPos;
                
                // on a wall       
                if(level[baseX-1][baseY-1] == blank && level[baseX][baseY-1] == blank && level[baseX+1][baseY-1] == blank &&
                   level[baseX-1][baseY] == wall && level[baseX][baseY] == wall && level[baseX+1][baseY] == wall &&
                   level[baseX-1][baseY+1] == floor && level[baseX][baseY+1] == floor && level[baseX+1][baseY+1] == floor)
                {
                    level[baseX][baseY] = upStairs;
                    return true;
                }
            }
        }
        else
        {
            for(size_t i = 0; i < rooms.size(); i++)
            {
                if(rooms[i].width >= 6 && rooms[i].height >= 6)
                {
                    size_t baseX = rooms[i].centre().first;
                    size_t baseY = rooms[i].centre().second;

                    if(level[baseX][baseY] != floor)
                        continue;

                    level[baseX][baseY] = upStairs;
                    
                    return true;
                }
            }
        }
            

        return false;
    }

    bool placeDownStairs(Level::Dun& level, const std::vector<Room>& rooms)
    {
        for(size_t i = rooms.size()-1; i != 0; i--)
        {
            if(rooms[i].width >= 6 && rooms[i].height >= 6)
            {
                size_t baseX = rooms[i].centre().first;
                size_t baseY = rooms[i].centre().second;

                if(level[baseX][baseY] != floor)
                        continue;

                level[baseX][baseY] = downStairs;
                
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
    Level::Dun generateTmp(size_t width, size_t height, size_t levelNum)
    {
        Level::Dun level(width, height);
        
        // Initialise whole dungeon to blank
        for(size_t x = 0; x < width; x++)
            for(size_t y = 0; y < height; y++)
                level[x][y] = blank;

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
        size_t fifteenPercent = (((float)rooms.size())/100.0)*15.0;
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
        for(size_t i = 0; i < rooms.size(); i++)
            drawRoom(rooms[i], level);
        
        // Bound corridoors with walls
        for(size_t x = 0; x < width; x++)
        {
            for(size_t y = 0; y < height; y++)
            {
                if(getXY(x, y, level) == blank)
                {
                    if(borders(x, y, floor, level))
                        level[x][y] = wall;
                }
            }
        }
        
        cleanup(level, rooms); 
        addDoors(level, rooms);
        
        // Make sure we always place stairs
        if(!(placeUpStairs(level, rooms, levelNum) && placeDownStairs(level, rooms)))
            return generateTmp(width, height, levelNum);

        return level;
    }

    bool isWall(size_t x, size_t y, const Level::Dun& level)
    {
        return getXY(x, y, level) == wall || getXY(x, y, level) == door || getXY(x, y, level) == upStairs;
    }
 
    void setPoint(int32_t x, int32_t y, size_t val, const Level::Dun& tmpLevel,  Level::Dun& level, const TileSet& tileset)
    {
        size_t newVal = val;

        if(val == tileset.xWall)
        {   
            if(getXY(x, y, tmpLevel) == door)
                newVal = tileset.xDoor;
            else if(getXY(x, y, tmpLevel) == upStairs)
                newVal = upStairs;
            else if(getXY(x, y+1, tmpLevel) == blank)
                newVal = tileset.outsideXWall;
            else if(getXY(x+1, y, tmpLevel) == floor)
                newVal = tileset.xWallEnd;
            else if(getXY(x-1, y, tmpLevel) == floor)
                newVal = tileset.leftCorner;
        }

        else if(val == tileset.yWall)
        {   
            if(getXY(x, y, tmpLevel) == door)
                newVal = tileset.yDoor;
            else if(getXY(x+1, y, tmpLevel) == blank)
                newVal = tileset.outsideYWall;
            else if(getXY(x, y+1, tmpLevel) == floor)
                newVal = tileset.yWallEnd;
            else if(getXY(x, y-1, tmpLevel) == floor)
                newVal = tileset.rightCorner;
        }

        else if(val == tileset.bottomCorner)
        {
            if(getXY(x+1, y+1, tmpLevel) == blank || getXY(x+1, y, tmpLevel) == blank || getXY(x, y+1, tmpLevel) == blank)
            {
                if(isWall(x, y+1, tmpLevel))
                    newVal = tileset.outsideYWall;
                else
                    newVal = tileset.outsideBottomCorner;
            }
            else if(isWall(x, y+1, tmpLevel))
                newVal = tileset.yWall;
        }

        else if(val == tileset.topCorner)
        {
            if(getXY(x+1, y+1, tmpLevel) == blank)
                newVal = tileset.outsideTopCorner;
        }

        else if(val == tileset.rightCorner)
        {
            if(getXY(x+1, y-1, tmpLevel) == blank  || getXY(x+1, y, tmpLevel) == blank || getXY(x, y-1, tmpLevel) == blank)
                newVal = tileset.outsideRightCorner;
        }

        else if(val == tileset.leftCorner)
        {
            if(getXY(x-1, y+1, tmpLevel) == blank  || getXY(x-1, y, tmpLevel) == blank || getXY(x, y+1, tmpLevel) == blank)
                newVal = tileset.outsideLeftCorner;
        }

        level[x][y] = newVal;
    }
    
    void placeMonsters(Level::Level& level, const DiabloExe::DiabloExe& exe, size_t dLvl)
    {
        std::vector<Level::Monster>& monsters = level.getMonsters();
        
        std::vector<const DiabloExe::Monster*> possibleMonsters = exe.getMonstersInLevel(dLvl);
        
        for(size_t i = 0; i < (level.height() + level.width())/2; i++)
        {
            Level::Monster m;
            do
            {
                m.xPos = randomInRange(1, level.width()-1);
                m.yPos = randomInRange(1, level.height()-1);
            }
            while(!level[m.xPos][m.yPos].passable());

            m.name = possibleMonsters[randomInRange(0, possibleMonsters.size()-1)]->monsterName;

            monsters.push_back(m);
        }
    }
 
    Level::Level* generate(size_t width, size_t height, size_t dLvl, const DiabloExe::DiabloExe& exe)
    {
        size_t levelNum = ((dLvl-1) / 4) + 1;

        Level::Dun tmpLevel = generateTmp(width, height, levelNum);

        Level::Dun level(width, height);

        std::stringstream ss; ss << "resources/tilesets/l" << levelNum << ".ini";
        TileSet tileset(ss.str());
         
        // Fill in isometric information (wall direction, etc), using flat tmpLevel as a base
        for(int32_t x = 0; x < (int32_t)width; x++)
        {
            for(int32_t y = 0; y < (int32_t)height; y++)
            {
                if(tmpLevel[x][y] == upStairs || tmpLevel[x][y] == downStairs)
                {
                    level[x][y] = tmpLevel[x][y];
                    continue;
                }

                if(isWall(x, y, tmpLevel))
                {
                    if(isWall(x+1, y, tmpLevel))
                    {
                        if(isWall(x, y+1, tmpLevel))
                            setPoint(x, y, tileset.topCorner, tmpLevel, level, tileset);
                        else
                        {
                            if(isWall(x, y-1, tmpLevel))
                                setPoint(x, y, tileset.leftCorner, tmpLevel, level, tileset);
                            else
                                setPoint(x, y, tileset.xWall, tmpLevel, level, tileset);
                        }
                    }
                    else if(isWall(x-1, y, tmpLevel))
                    {
                        if(isWall(x, y-1, tmpLevel))
                            setPoint(x, y, tileset.bottomCorner, tmpLevel, level, tileset);
                        else
                        {
                            if(isWall(x, y+1, tmpLevel))
                                setPoint(x, y, tileset.rightCorner, tmpLevel, level, tileset);
                            else
                                setPoint(x, y, tileset.xWall, tmpLevel, level, tileset);
                        }
                    }
                    else
                    {
                        setPoint(x, y, tileset.yWall, tmpLevel, level, tileset);
                    }
                }
                else
                {
                    if(tmpLevel[x][y] == blank)
                        level[x][y] = tileset.blank;
                    else
                        level[x][y] = tileset.floor;
                }
            }
        }

        std::pair<size_t, size_t> downStairsPoint;
        std::pair<size_t, size_t> upStairsPoint;
        
        // Add in some random aesthetic variation
        for(int32_t x = 0; x < (int32_t)width; x++)
        {
            for(int32_t y = 0; y < (int32_t)height; y++)
            {
                if(level[x][y] == upStairs)
                {
                    level[x-1][y-1] = tileset.upStairs1;
                    level[x][y-1] = tileset.upStairs2;
                    level[x+1][y-1] = tileset.upStairs3;

                    level[x-1][y] = tileset.upStairs4;
                    level[x][y] = tileset.upStairs5;
                    level[x+1][y] = tileset.upStairs6;

                    level[x-1][y+1] = tileset.upStairs7;
                    level[x][y+1] = tileset.upStairs8;
                    level[x+1][y+1] = tileset.upStairs9;

                    upStairsPoint = std::make_pair(x*2,y*2);
                }
                else if(level[x][y] == downStairs)
                {
                    level[x-1][y-1] = tileset.downStairs1;
                    level[x][y-1] = tileset.downStairs2;
                    level[x+1][y-1] = tileset.downStairs3;

                    level[x-1][y] = tileset.downStairs4;
                    level[x][y] = tileset.downStairs5;
                    level[x+1][y] = tileset.downStairs6;

                    level[x-1][y+1] = tileset.downStairs7;
                    level[x][y+1] = tileset.downStairs8;
                    level[x+1][y+1] = tileset.downStairs9;

                    downStairsPoint = std::make_pair(x*2,y*2);
                }
                else
                {
                    level[x][y] = tileset.getRandomTile(level[x][y]);
                }
            }
        }
        
        ss.str(""); ss << "levels/l" << levelNum << "data/l" << levelNum << ".cel";
        std::string celPath = ss.str();

        ss.str(""); ss << "levels/l" << levelNum << "data/l" << levelNum << ".til";
        std::string tilPath = ss.str();

        ss.str(""); ss << "levels/l" << levelNum << "data/l" << levelNum << ".min";
        std::string minPath = ss.str();

        ss.str(""); ss << "levels/l" << levelNum << "data/l" << levelNum << ".sol";
        std::string solPath = ss.str();
                
        Level::Level* retval = new Level::Level(level, tilPath, minPath, solPath, celPath, downStairsPoint, upStairsPoint, tileset.getDoorMap());
        placeMonsters(*retval, exe, dLvl); 
        
        return retval;
    }
}
