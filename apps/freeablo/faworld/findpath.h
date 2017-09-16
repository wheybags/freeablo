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


namespace FAWorld
{
    class GameLevelImpl;
    std::vector<std::pair<int32_t, int32_t>> pathFind(GameLevelImpl* level, std::pair<int32_t, int32_t> start, std::pair<int32_t, int32_t>& goal, bool& bArrivable, bool
                                                      findAdjacent);
}
#endif /* COMPONENTS_LEVEL_PATHFINDING_H_ */
