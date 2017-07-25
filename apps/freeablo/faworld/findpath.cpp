#include "findpath.h"

namespace std 
{
    template <>
    struct hash<pair<int, int> > {
        inline size_t operator()(const pair<size_t, size_t>& location) const {
            int x, y;
            x = location.first;
            y = location.second;
            return x * 1812433253 + y;
        }
    };
}

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

        std::vector<Location> results;

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

    bool AStarSearch(
        GameLevelImpl* level,
        Location start,
        Location goal,
        std::unordered_map<Location, Location>& came_from,
        std::unordered_map<Location, int>& costSoFar)
    {
        PriorityQueue<Location> frontier;
        frontier.put(start, 0);
        came_from[start] = start;
        costSoFar[start] = 0;
        while (!frontier.empty())
        {
            Location current = frontier.get();

            // Early exit
            if (current == goal)
            {
                return true;
            }

            std::vector<Location> neighborsContainer = neighbors(level, current);
            for (std::vector<Location>::iterator it = neighborsContainer.begin(); it != neighborsContainer.end(); it++)
            {
                int new_cost = costSoFar[current] + 1; //graph.cost(current, next);
                Location next = *it;

                if (!costSoFar.count(next) || new_cost < costSoFar[next])
                {
                    costSoFar[next] = new_cost;
                    int priority = new_cost + heuristic(next, goal);
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

    std::vector<Location> pathFind(GameLevelImpl* level, Location start, Location& goal, bool& bArrivable)
    {
        std::unordered_map<Location, int> costSoFar;
        std::unordered_map<Location, Location> cameFrom;

        bArrivable = AStarSearch(level, start, goal, cameFrom, costSoFar);
        if (!bArrivable) 
            goal = findClosesPointToGoal(level, start, goal, cameFrom);

        return reconstructPath(start, goal, cameFrom);
    }
}
