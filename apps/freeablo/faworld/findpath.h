#pragma once

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <misc/point.h>
#include <queue>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace FAWorld
{
    class GameLevelImpl;

    std::vector<Misc::Point> pathFind(GameLevelImpl* level, const Misc::Point& start, Misc::Point& goal, bool& bArrivable, bool findAdjacent);
} // namespace FAWorld
