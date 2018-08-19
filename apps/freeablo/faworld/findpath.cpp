#include "findpath.h"
#include "gamelevel.h"
#include <misc/stdhashes.h>

namespace FAWorld
{
    template <typename T, typename Number = size_t> struct PriorityQueue
    {
        typedef std::pair<Number, T> PQElement;
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

        inline bool empty() { return elements.empty(); }

        inline void put(T item, Number priority) { elements.emplace(priority, item); }

        inline T get()
        {
            T best_item = elements.top().second;
            elements.pop();
            return best_item;
        }
    };

    bool inBounds(GameLevelImpl* level, Misc::Point location)
    {
        int x = location.x;
        int y = location.y;
        return 0 <= x && x < (int)level->width() && 0 <= y && y < (int)level->height();
    }

    Misc::Points neighbors(GameLevelImpl* level, Misc::Point location)
    {
        int x = location.x;
        int y = location.y;

        Misc::Points result;
        result.reserve(9);

        for (int32_t dy = -1; dy <= 1; dy++)
        {
            for (int32_t dx = -1; dx <= 1; dx++)
            {
                Misc::Point next(x + dx, y + dy);
                if (inBounds(level, next) && level->isPassable(next))
                    result.push_back(next);
            }
        }

        return result;
    }

    int heuristic(Misc::Point a, Misc::Point b)
    {
        int dx = abs(b.x - a.x);
        int dy = abs(b.y - a.y);

        return dx + dy;
    }

    template <typename T> class Array2D
    {
    public:
        Array2D(int32_t width, int32_t height) : mData(width * height), mWidth(width), mHeight(height) {}

        Array2D(int32_t width, int32_t height, T defaultVal) : mData(width * height, defaultVal), mWidth(width), mHeight(height) {}

        T& get(int32_t x, int32_t y) { return mData.at(x + y * mWidth); }

        int32_t width() { return mWidth; }

        int32_t height() { return mHeight; }

    private:
        std::vector<T> mData;
        int32_t mWidth;
        int32_t mHeight;
    };

    bool AStarSearch(GameLevelImpl* level, Misc::Point start, Misc::Point& goal, std::unordered_map<Misc::Point, Misc::Point>& came_from, bool findAdjacent)
    {
        auto goalPassable = level->isPassable(goal);
        PriorityQueue<Misc::Point> frontier;
        frontier.put(start, 0);
        came_from[start] = start;

        Array2D<int32_t> costSoFar(level->width(), level->height(), -1);
        costSoFar.get(start.x, start.y) = 0;

        int32_t iterations = 0;
        while (!frontier.empty() && iterations < 500)
        {
            iterations++;
            Misc::Point current = frontier.get();

            // Early exit
            if (current == goal)
                return true;
            if (findAdjacent || !goalPassable)
            {
                if (abs(goal.x - current.x) <= 1 && abs(goal.y - current.y) <= 1)
                {
                    goal = current;
                    return true;
                }
            }

            Misc::Points neighborsContainer = neighbors(level, current);
            for (auto it = neighborsContainer.begin(); it != neighborsContainer.end(); it++)
            {
                int32_t new_cost = costSoFar.get(current.x, current.y) + 1; // graph.cost(current, next);
                Misc::Point next = *it;

                if (costSoFar.get(next.x, next.y) == -1 || new_cost < costSoFar.get(next.x, next.y))
                {
                    costSoFar.get(next.x, next.y) = new_cost;
                    int32_t priority = new_cost + heuristic(next, goal);
                    frontier.put(next, priority);
                    came_from[next] = current;
                }
            }
        }

        return false;
    }

    Misc::Points reconstructPath(Misc::Point start, Misc::Point goal, std::unordered_map<Misc::Point, Misc::Point>& cameFrom)
    {
        Misc::Points path;
        Misc::Point current = goal;
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

    Misc::Point findClosesPointToGoal(GameLevelImpl* level, Misc::Point start, Misc::Point goal, std::unordered_map<Misc::Point, Misc::Point>& cameFrom)
    {
        // int dx = abs(goal.first - start.first);
        // int dy = abs(goal.second - start.second);
        UNUSED_PARAM(start);
        int minDistance = level->width() * level->width() + level->height() * level->height();

        Misc::Point result(-1, -1);

        for (auto it = cameFrom.begin(); it != cameFrom.end(); it++)
        {
            int tmpX = abs(it->first.x - goal.x);
            int tmpY = abs(it->first.y - goal.y);

            int distance = tmpX * tmpX + tmpY * tmpY;
            if (distance < minDistance)
            {
                minDistance = distance;
                result = it->first;
            }
        }

        return result;
    }

    Misc::Points pathFind(GameLevelImpl* level, const Misc::Point& start, Misc::Point& goal, bool& bArrivable, bool findAdjacent)
    {
        std::unordered_map<Misc::Point, Misc::Point> cameFrom;

        bArrivable = AStarSearch(level, start, goal, cameFrom, findAdjacent);
        if (!bArrivable)
            return {};

        return reconstructPath(start, goal, cameFrom);
    }
}
