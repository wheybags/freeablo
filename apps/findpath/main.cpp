#include <level/findpath.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <string>

using namespace std::chrono;
using namespace std;

namespace Level
{
    class LevelImpl : public Level
    {

    public:

        static const int MAP_SIZE = 20;

        LevelImpl()
        {
            generateMap();
            //drawMap();
        }

        size_t width()
        {
            return MAP_SIZE;
        }

        size_t height()
        {
            return MAP_SIZE;
        }

        bool isPassable(int x, int y) const
        {
            return gameMap[y][x] == 0 ? true : false;
        }

    private:
        int gameMap[MAP_SIZE][MAP_SIZE];

    private:

        void generateMap()
        {
            memset(gameMap, 0, MAP_SIZE*MAP_SIZE*sizeof(int));
            for(int i = 0 ; i < MAP_SIZE ; i++)
            {
                gameMap[0][i] = 1;
                gameMap[MAP_SIZE - 1][i] = 1;
            }

            gameMap[MAP_SIZE / 2][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 1][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 2][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 3][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 4][MAP_SIZE / 2] = 1;

            for(int i = 3; i <= MAP_SIZE - 1; i++)
                for(int j = 14; j <= 16; j++)
                    if(i == MAP_SIZE - 3)
                        gameMap[i][j] = 0;
                    else
                        gameMap[i][j] = 1;

        }

        void drawMap()
        {
            for(int y = 0 ; y < MAP_SIZE; y++)
            {
                for(int x = 0 ; x < MAP_SIZE ; x++)
                {
                    if(gameMap[y][x])
                        cout << "# ";
                    else
                        cout << ". ";
                }

                cout << endl;
            }
        }
    };
}

using namespace Level;

ostream& operator<<(ostream& os, FindPath::Location & location )
{
    os << "(" << location.first << "," << location.second << ")";
    return os;
}

void drawPath(::Level::Level& graph, int field_width,
               unordered_map<FindPath::Location, int>* distances=0,
               unordered_map<FindPath::Location, FindPath::Location>* point_to=0,
               vector<FindPath::Location>* path=0)
{
    for (int y = 0; y != LevelImpl::MAP_SIZE; ++y)
    {
        for (int x = 0; x != LevelImpl::MAP_SIZE; ++x)
        {
            FindPath::Location id;
            id.first = x;
            id.second = y;
            cout << left << setw(field_width);
            if (!graph.isPassable(x,y))
            {
                cout << string(field_width, '#');
            }
            else if (point_to != 0 && point_to->count(id))
            {
                int x2, y2;
                x2 = (*point_to)[id].first;
                y2 = (*point_to)[id].second;

                if (x2 == x + 1) { cout << "\u2192 "; }
                else if (x2 == x - 1) { cout << "\u2190 "; }
                else if (y2 == y + 1) { cout << "\u2193 "; }
                else if (y2 == y - 1) { cout << "\u2191 "; }
                else { cout << "* "; }
            }
            else if (distances != 0 && distances->count(id))
            {
                cout << (*distances)[id];
            }
            else if (path != 0 && find(path->begin(), path->end(), id) != path->end())
            {
                cout << '@';
            }
            else
            {
                cout << '.';
            }
        }
        cout << endl;
    }
}

int main()
{
    ::Level::Level * level = new LevelImpl();
    FindPath findPath(level);

    FindPath::Location start,goal;
    vector<FindPath::Location> path;

    int caseId = 1;
    cout << "Select case ID [1-6]";
    cin >> caseId;

    switch(caseId)
    {
    case 1:
        start = FindPath::Location(2,4);
        goal = FindPath::Location(18,5);
        break;
    case 2:
        start = FindPath::Location(18,5);
        goal = FindPath::Location(2,3);
        break;
    case 3:
        start = FindPath::Location(3,10);
        goal = FindPath::Location(18,10);
        break;
    case 4:
        // This case uses "findClosesPointToGoal"
        start = FindPath::Location(3,10);
        goal = FindPath::Location(15,10);
        break;
    case 5:
        // This case uses "findClosesPointToGoal"
        start = FindPath::Location(18,7);
        goal = FindPath::Location(10,10);
        break;
    default:
        // This case uses "findClosesPointToGoal"
        start = FindPath::Location(18,10);
        goal = FindPath::Location(10,10);
        break;
    }

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    path = findPath.find(start, goal);

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    drawPath(*level,2,0,0,&path);

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    cout << "Duration: " << duration << " us" << endl;


    delete level;
    return 0;
}
