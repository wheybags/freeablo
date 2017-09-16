#include "../../freeablo/apps/freeablo/faworld/findpath.h"
#include "../../freeablo/apps/freeablo/faworld/gamelevel.h"
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
#include <misc/stdhashes.h>

using namespace std::chrono;
using namespace std;

namespace FAWorld
{
    class LevelImpl : public GameLevelImpl
    {

    public:

        static const int MAP_SIZE = 20;

        LevelImpl()
        {
            generateMap();
            //drawMap();
        }

        int32_t width() const
        {
            return MAP_SIZE;
        }

        int32_t height() const
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
            memset(gameMap, 0, MAP_SIZE*MAP_SIZE * sizeof(int));
            for (int i = 0; i < MAP_SIZE; i++)
            {
                gameMap[0][i] = 1;
                gameMap[MAP_SIZE - 1][i] = 1;
            }

            gameMap[MAP_SIZE / 2][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 1][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 2][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 3][MAP_SIZE / 2] = 1;
            gameMap[MAP_SIZE / 2 + 4][MAP_SIZE / 2] = 1;

            for (int i = 3; i <= MAP_SIZE - 1; i++)
                for (int j = 14; j <= 16; j++)
                    if (i == MAP_SIZE - 3)
                        gameMap[i][j] = 0;
                    else
                        gameMap[i][j] = 1;

        }

        void drawMap()
        {
            for (int y = 0; y < MAP_SIZE; y++)
            {
                for (int x = 0; x < MAP_SIZE; x++)
                {
                    if (gameMap[y][x])
                        cout << "# ";
                    else
                        cout << ". ";
                }

                cout << endl;
            }
        }
    };
}

using namespace FAWorld;

ostream& operator<<(ostream& os, std::pair<int32_t, int32_t> & location)
{
    os << "(" << location.first << "," << location.second << ")";
    return os;
}

void drawPath(::GameLevel::GameLevelImpl& graph, int field_width,
    unordered_map<std::pair<int32_t, int32_t>, int>* distances = 0,
    unordered_map<std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>>* point_to = 0,
    vector<std::pair<int32_t, int32_t>>* path = 0)
{
    for (int y = 0; y != LevelImpl::MAP_SIZE; ++y)
    {
        for (int x = 0; x != LevelImpl::MAP_SIZE; ++x)
        {
            std::pair<int32_t, int32_t> id;
            id.first = x;
            id.second = y;
            cout << left << setw(field_width);
            if (!graph.isPassable(x, y))
            {
                cout << string(field_width, '#');
            }
            else if (point_to != 0 && point_to->count(id))
            {
                int x2, y2;
                x2 = (*point_to)[id].first;
                y2 = (*point_to)[id].second;

                if (x2 == x + 1) { cout << u8"\u2192 "; }
                else if (x2 == x - 1) { cout << u8"\u2190 "; }
                else if (y2 == y + 1) { cout << u8"\u2193 "; }
                else if (y2 == y - 1) { cout << u8"\u2191 "; }
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
    using namespace FAWorld;
    FAWorld::LevelImpl * level = new LevelImpl();

    std::pair<int32_t, int32_t> start, goal;
    vector<std::pair<int32_t, int32_t>> path;

    int caseId = 1;
    cout << "Select case ID [1-6]";
    cin >> caseId;

    switch (caseId)
    {
    case 1:
        start = std::pair<int32_t, int32_t>(2, 4);
        goal = std::pair<int32_t, int32_t>(18, 5);
        break;
    case 2:
        start = std::pair<int32_t, int32_t>(18, 5);
        goal = std::pair<int32_t, int32_t>(2, 3);
        break;
    case 3:
        start = std::pair<int32_t, int32_t>(3, 10);
        goal = std::pair<int32_t, int32_t>(18, 10);
        break;
    case 4:
        // This case uses "findClosesPointToGoal"
        start = std::pair<int32_t, int32_t>(3, 10);
        goal = std::pair<int32_t, int32_t>(15, 10);
        break;
    case 5:
        // This case uses "findClosesPointToGoal"
        start = std::pair<int32_t, int32_t>(18, 7);
        goal = std::pair<int32_t, int32_t>(10, 10);
        break;
    default:
        // This case uses "findClosesPointToGoal"
        start = std::pair<int32_t, int32_t>(18, 10);
        goal = std::pair<int32_t, int32_t>(10, 10);
        break;
    }

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    bool bArrivable = false;
    path = pathFind(level, start, goal, bArrivable, false);

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    drawPath(*level, 2, 0, 0, &path);

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "Duration: " << duration << " us" << endl;


    delete level;
    return 0;
}
