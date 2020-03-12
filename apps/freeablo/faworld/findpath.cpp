#include "findpath.h"
#include "gamelevel.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <misc/array2d.h>
#include <misc/stdhashes.h>
#include <queue>

namespace
{
    const int STRAIGHT_WEIGHT = 10;
    const int DIAGONAL_WEIGHT = 14;

    int distanceCost(const Misc::Point& a, const Misc::Point& b) { return (a.x != b.x && a.y != b.y) ? DIAGONAL_WEIGHT : STRAIGHT_WEIGHT; }
}

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

    Misc::Points neighbors(GameLevelImpl* level, const Actor* actor, const Misc::Point& location)
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
                if (inBounds(level, next) && level->isPassable(next, actor))
                    result.push_back(next);
            }
        }

        return result;
    }

    size_t heuristic(Misc::Point a, Misc::Point b)
    {
        auto dx = abs(b.x - a.x);
        auto dy = abs(b.y - a.y);

        auto straight = std::abs(dx - dy);
        auto diagonal = std::max(dx, dy) - straight;

        return straight * STRAIGHT_WEIGHT + diagonal * DIAGONAL_WEIGHT;
    }

    bool AStarSearch(GameLevelImpl* level,
                     const Actor* actor,
                     Misc::Point start,
                     Misc::Point& goal,
                     std::unordered_map<Misc::Point, Misc::Point>& came_from,
                     bool findAdjacent)
    {
        auto goalPassable = level->isPassable(goal, actor);
        PriorityQueue<Misc::Point> frontier;
        frontier.put(start, 0);
        came_from[start] = start;

        static Misc::Array2D<int32_t> costSoFar;
        costSoFar.resize(level->width(), level->height());
        memset(costSoFar.data(), 0xff, level->width() * level->height() * sizeof(int32_t));

        costSoFar.get(start.x, start.y) = 0;

        int32_t iterations = 0;
        while (!frontier.empty() && iterations < 1000)
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

            for (const auto& next : neighbors(level, actor, current))
            {
                auto new_cost = costSoFar.get(current.x, current.y) + distanceCost(current, next);

                if (costSoFar.get(next.x, next.y) == -1 || new_cost < costSoFar.get(next.x, next.y))
                {
                    costSoFar.get(next.x, next.y) = new_cost;
                    frontier.put(next, new_cost + heuristic(next, goal));
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
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        return path;
    }

    Misc::Points pathFind(GameLevelImpl* level, const Actor* actor, const Misc::Point& start, const Misc::Point& goal, bool& bArrivable, bool findAdjacent)
    {
        auto adjustedGoal = goal;
        std::unordered_map<Misc::Point, Misc::Point> cameFrom;

        bArrivable = AStarSearch(level, actor, start, adjustedGoal, cameFrom, findAdjacent);
        if (!bArrivable)
            return {};

        return reconstructPath(start, adjustedGoal, cameFrom);
    }
}
