#include "findpath.h"

namespace FAWorld
{

    FindPath* FindPath::instance = nullptr;

    FindPath::FindPath(GameLevelImpl * level)
        :FindPath::FindPath()
    {
        this->level = level;
    }

    FindPath::FindPath() {
        directions.push_back(make_pair(-1, 0));
        directions.push_back(make_pair(1, 0));
        directions.push_back(make_pair(0, -1));
        directions.push_back(make_pair(0, 1));
        directions.push_back(make_pair(-1, -1));
        directions.push_back(make_pair(1, -1));
        directions.push_back(make_pair(1, 1));
        directions.push_back(make_pair(-1, 1));
        sizeOfDirections = directions.size();
    }

    bool FindPath::inBounds(Location location)
    {
        int x = location.first;
        int y = location.second;
        return 0 <= x && x < (int)level->width() && 0 <= y && y < (int)level->height();
    }

    bool FindPath::passable(Location location)
    {
        bool isPassable = level->isPassableFor(location.first, location.second, mActor);
        return isPassable;
    }

    vector<FindPath::Location> FindPath::neighbors(Location location)
    {
        int dx, dy;
        int x = location.first;
        int y = location.second;

        vector<Location> results;

        for (unsigned int i = 0; i < sizeOfDirections; i++)
        {
            dx = directions[i].first;
            dy = directions[i].second;

            Location next(x + dx, y + dy);
            if (inBounds(next) && passable(next))
                results.push_back(next);
        }

        return results;
    }

    int FindPath::heuristic(FindPath::Location a, FindPath::Location b)
    {
        int dx = abs(b.first - a.first);
        int dy = abs(b.second - a.second);

        return dx + dy;
    }

    bool FindPath::AStarSearch(
      FindPath::Location start,
      FindPath::Location& goal,
      unordered_map<FindPath::Location, FindPath::Location>& came_from,
      unordered_map<FindPath::Location, int>& costSoFar)
    {
        PriorityQueue<Location> frontier;
        frontier.put(start, 0);
        came_from[start] = start;
        costSoFar[start] = 0;
        while (!frontier.empty())
        {
            Location current = frontier.get();

            // Early exit
            if (passable (goal))
              {
                if (current == goal)
                  return true;
              }
            else // if we didn't reach goal and it's unreachable but we reach neighbour's tile, then it's our new goal
              {
                if (abs (goal.first - current.first) <= 1 && abs (goal.second - current.second) <= 1)
                    {
                      goal = current;
                      return true;
                    }
              }

            vector<Location> neighborsContainer = neighbors(current);
            for (vector<Location>::iterator it = neighborsContainer.begin(); it != neighborsContainer.end(); it++)
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

    vector<FindPath::Location> FindPath::reconstructPath(
        FindPath::Location start,
        FindPath::Location goal,
        unordered_map<FindPath::Location, FindPath::Location>& cameFrom)
    {
        vector<Location> path;
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

    FindPath::Location FindPath::findClosesPointToGoal(FindPath::Location start, FindPath::Location goal, unordered_map<FindPath::Location, FindPath::Location> & cameFrom)
    {
        //int dx = abs(goal.first - start.first);
        //int dy = abs(goal.second - start.second);
        UNUSED_PARAM(start);
        int minDistance = level->width() * level->width() + level->height() * level->height();

        Location result(-1, -1);

        for (unordered_map<Location, Location>::iterator it = cameFrom.begin(); it != cameFrom.end(); it++)
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

    vector<FindPath::Location> FindPath::find(FindPath::Location start, FindPath::Location& goal, bool& bArrivable, const Actor *actor)
    {
        mActor = actor;
        unordered_map<Location, int> costSoFar;
        unordered_map<Location, Location> cameFrom;

        // looks like original game does the following: it reaches one of neighbor tiles (possibly one with sqrt (2) distance, not 1 if it's closer)
        // of the target if target itself is not passable, otherwise it does nothing. Also resolution of its search is much lower.
        // the reaching neighbor tile fastest way is important since it allows to talk with/perform melee attack on target

        bool found = AStarSearch(start, goal, cameFrom, costSoFar);
        if (!found) { // so according to above explanation part in this branch is not performed by original game, I'll leave it be for now
            goal = findClosesPointToGoal(start, goal, cameFrom);
            bArrivable = false;
        }
        else
        {
            bArrivable = true;
        }

        return reconstructPath(start, goal, cameFrom);
    }

    FindPath * FindPath::get(GameLevelImpl* level)
    {
        if (instance == nullptr)
            instance = new FindPath();
        instance->level = level;
        return instance;
    }
}
