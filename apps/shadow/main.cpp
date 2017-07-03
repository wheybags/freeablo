#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shadowcaster/shadowcaster.h>

using namespace ShadowCaster;
using namespace std;

class Map : public TransparencyMap
{
public:
    ~Map() {}

    bool isTransparent(int32_t x, int32_t y) const
    {
        bool map[][15] = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
        };
        // data map is transposed
        return map[y][x];
    }

    size_t height() const { return 15; }
    size_t width() const { return 15; }
};


int main()
{
    std::pair<int32_t, int32_t> start{8, 3};
    Map* map = new Map();
    Scanner* scanner = new Scanner(map, start, 10);

    auto visible = scanner->getVisibleTiles();


    for (int y = 0; y < map->height(); y++) {
        for (int x = 0; x < map->width(); x++) {
            if (x == start.first && y == start.second) {
                cout << '@';
            } else if (!map->isTransparent(x, y)) {
                cout << '#';
            } else if (visible.find({x, y}) != visible.end())  {
                cout << '.';
            } else {
                cout << 's';
            }
        }
        cout << endl;
    }


    delete map;
    delete scanner;

    return 0;
}
