#include "findpath.h"
#include <misc/stdhashes.h>
#include "gamelevel.h"

namespace FAWorld
{
    template<typename T, typename Number = size_t>
    struct PriorityQueue
    {
        typedef std::pair<Number, T> PQElement;
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement> > elements;

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

    typedef std::pair<int32_t, int32_t> Location;

    bool inBounds(GameLevelImpl* level, Location location)
    {
        int x = location.first;
        int y = location.second;
        return 0 <= x && x < (int)level->width() && 0 <= y && y < (int)level->height();
    }


    std::vector<Location> neighbors(GameLevelImpl* level, Location location)
    {
        int x = location.first;
        int y = location.second;

        std::vector<Location> results(0);
        results.reserve(9);

        for (int32_t dy = -1; dy <= 1; dy++)
        {
            for (int32_t dx = -1; dx <= 1; dx++)
            {
                Location next(x + dx, y + dy);
                if (inBounds(level, next) && level->isPassable(next.first, next.second))
                    results.push_back(next);
            }
        }

        return results;
    }

    int heuristic(Location a, Location b)
    {
        int dx = abs(b.first - a.first);
        int dy = abs(b.second - a.second);

        return dx + dy;
    }

    template<typename T>
    class Array2D
    {
    public:
        Array2D(int32_t width, int32_t height) :
            mData(width*height),
            mWidth(width),
            mHeight(height)
        {}

        Array2D(int32_t width, int32_t height, T defaultVal) :
            mData(width*height, defaultVal),
            mWidth(width),
            mHeight(height)
        {}

        T& get(int32_t x, int32_t y)
        {
            return mData.at(x + y*mHeight);
        }

        int32_t width()
        {
            return mWidth;
        }

        int32_t height()
        {
            return mHeight;
        }

    private:
        std::vector<T> mData;
        int32_t mWidth;
        int32_t mHeight;
    };

    bool AStarSearch(
        GameLevelImpl* level,
        Location start,
        Location& goal,
        std::unordered_map<Location, Location>& came_from, bool findAdjacent
    )
    {
        auto goalPassable = level->isPassable(goal.first, goal.second);
        PriorityQueue<Location> frontier;
        frontier.put(start, 0);
        came_from[start] = start;

        Array2D<int32_t> costSoFar(level->width(), level->height(), -1);
        costSoFar.get(start.first, start.second) = 0;

        int32_t iterations = 0;
        while (!frontier.empty() && iterations < 500)
        {
            iterations++;
            Location current = frontier.get();

            // Early exit
            if (current == goal)
                return true;
            if (findAdjacent || !goalPassable)
               {
                 if (abs (goal.first - current.first) <= 1 && abs (goal.second - current.second) <= 1)
                     {
                       goal = current;
                       return true;
                     }
               }


            std::vector<Location> neighborsContainer = std::move(neighbors(level, current));
            for (std::vector<Location>::iterator it = neighborsContainer.begin(); it != neighborsContainer.end(); it++)
            {
                int32_t new_cost = costSoFar.get(current.first, current.second) + 1; //graph.cost(current, next);
                Location next = *it;

                if (costSoFar.get(next.first, next.second) == -1 || new_cost < costSoFar.get(next.first, next.second))
                {
                    costSoFar.get(next.first, next.second) = new_cost;
                    int32_t priority = new_cost + heuristic(next, goal);
                    frontier.put(next, priority);
                    came_from[next] = current;
                }
            }
        }

        return false;
    }

    std::vector<Location> reconstructPath(
        Location start,
        Location goal,
        std::unordered_map<Location, Location>& cameFrom)
    {
        std::vector<Location> path;
        Location current = goal;
        path.push_back(current);
        while (current != start)
        {
            current = cameFrom[current];
            if (current != start)
                path.push_back(current);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    Location findClosesPointToGoal(GameLevelImpl* level, Location start, Location goal, std::unordered_map<Location, Location> & cameFrom)
    {
        //int dx = abs(goal.first - start.first);
        //int dy = abs(goal.second - start.second);
        UNUSED_PARAM(start);
        int minDistance = level->width() * level->width() + level->height() * level->height();

        Location result(-1, -1);

        for (std::unordered_map<Location, Location>::iterator it = cameFrom.begin(); it != cameFrom.end(); it++)
        {
            int tmpX = abs(it->first.first - goal.first);
            int tmpY = abs(it->first.second - goal.second);

            int distance = tmpX*tmpX + tmpY*tmpY;
            if (distance < minDistance)
            {
                minDistance = distance;
                result = it->first;
            }
        }

        return result;
    }

    std::vector<Location> pathFind(GameLevelImpl* level, Location start, Location& goal, bool& bArrivable, bool findAdjacent)
    {
        std::unordered_map<Location, Location> cameFrom;

        bArrivable = AStarSearch(level, start, goal, cameFrom, findAdjacent);
        if (!bArrivable) return {};

        return reconstructPath(start, goal, cameFrom);
    }
}
