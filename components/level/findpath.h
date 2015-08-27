#ifndef COMPONENTS_LEVEL_PATHFINDING_H_
#define COMPONENTS_LEVEL_PATHFINDING_H_

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
#include <unordered_map>
#include <unordered_set>
#include <functional>


using namespace std;

namespace std {

    template <>
    struct hash<pair<int,int> > {
        inline size_t operator()(const pair<size_t,size_t>& location) const {
            int x, y;
            x = location.first;
            y = location.second;
            return x * 1812433253 + y;
        }
    };
}

template<typename T, typename Number=size_t>
struct PriorityQueue
{
    typedef pair<Number, T> PQElement;
    priority_queue<PQElement, vector<PQElement>, std::greater<PQElement> > elements;

    inline bool empty() { return elements.empty(); }

    inline void put(T item, Number priority)
    {
        elements.emplace(priority, item);
    }

    inline T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

namespace Level
{

class Level
{
public:
    virtual ~Level() {}
    virtual size_t width() = 0;
    virtual size_t height() = 0;
    virtual bool isPassable(int x, int y) const = 0;
};


class FindPath {
public:

    typedef pair<int, int> Location;

    FindPath(Level * level);
	vector<Location> find(Location start, Location goal);

private:

	bool inBounds(Location location);
    bool passable(Location location);
    vector<Location> neighbors(Location location);
    int heuristic(Location a, Location b);
    bool AStarSearch(Location start, Location goal, unordered_map<Location, Location>& cameFrom, unordered_map<Location, int>& costSoFar);
    vector<Location> reconstructPath(Location start, Location goal, unordered_map<Location, Location>& cameFrom);
    Location findClosesPointToGoal(Location start, Location goal, unordered_map<Location, Location> & cameFrom);

    Level * level;
	vector<Location> directions;
	unsigned int sizeOfDirections;
};

}
#endif /* COMPONENTS_LEVEL_PATHFINDING_H_ */
