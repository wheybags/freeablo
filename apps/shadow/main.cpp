#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shadowcaster/shadowcaster.h>

using namespace ShadowCaster;
using namespace std;

class Map : public VisibilityMap
{
public:
    ~Map() {}

    bool isTransparent(int32_t x, int32_t y) const
    {
        bool map[][10] = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
        };
        // data map is transposed
        return map[y][x];
    }

    int getHeight() const { return 10; }
    int getWidth() const { return 10; }
};


int main()
{
    Map* map = new Map();
    Scanner* scanner = new Scanner(map, {5, 3});

    auto visible = scanner->getVisibleTiles();


    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            if (x == 5 && y == 3) {
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
