#include "findpath.h"

namespace Level
{

FindPath::FindPath(Level * level)
	: level(level)
{
    directions.push_back(make_pair(-1,0));
    directions.push_back(make_pair(1,0));
    directions.push_back(make_pair(0,-1));
    directions.push_back(make_pair(0,1));
    directions.push_back(make_pair(-1,-1));
    directions.push_back(make_pair(1,-1));
    directions.push_back(make_pair(1,1));
    directions.push_back(make_pair(-1,1));
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
    bool isPassable = level->isPassable(location.first, location.second);
    return isPassable;
}

vector<FindPath::Location> FindPath::neighbors(Location location)
{
    int dx, dy;
    int x = location.first;
    int y = location.second;

    vector<Location> results;

    for(unsigned int i = 0 ; i < sizeOfDirections ; i++)
    {
        dx = directions[i].first;
        dy = directions[i].second;

        Location next(x + dx, y + dy);
        if(inBounds(next) && passable(next))
            results.push_back(next);
    }

    return results;
}

int FindPath::heuristic(FindPath::Location a, FindPath::Location b)
{
    int x1, y1, x2, y2;
    x1 = a.first;
    y1 = a.second;
    x2 = b.first;
    y2 = b.second;
    return abs(x1 - x2) + abs(y1 - y2);
}

bool FindPath::AStarSearch(
        FindPath::Location start,
        FindPath::Location goal,
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
        if(current == goal)
        {
            return true;
        }

        vector<Location> neighborsContainer = neighbors(current);
        for(vector<Location>::iterator it = neighborsContainer.begin(); it != neighborsContainer.end() ; it++)
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
        if(current != start)
        	path.push_back(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

FindPath::Location FindPath::findClosesPointToGoal(FindPath::Location start, FindPath::Location goal, unordered_map<FindPath::Location, FindPath::Location> & cameFrom)
{
    //int dx = abs(goal.first - start.first);
    //int dy = abs(goal.second - start.second);
    int minDistance = level->width() * level->width() + level->height() * level->height();

    Location result(-1,-1);

    for(unordered_map<Location, Location>::iterator it = cameFrom.begin() ; it != cameFrom.end() ; it++)
    {
        int tmpX = abs(it->first.first - goal.first);
        int tmpY = abs(it->first.second- goal.second);

        if(tmpX*tmpX + tmpY*tmpY < minDistance)
        {
            minDistance = tmpX*tmpX + tmpY*tmpY;
            result = it->first;
        }

    }

    return result;
}

vector<FindPath::Location> FindPath::find(FindPath::Location start, FindPath::Location goal)
{
	unordered_map<Location, int> costSoFar;
	unordered_map<Location, Location> cameFrom;

	bool found = AStarSearch(start, goal, cameFrom, costSoFar);
	if(!found)
		goal = findClosesPointToGoal(start, goal , cameFrom);

	return reconstructPath(start, goal , cameFrom);
}

}
