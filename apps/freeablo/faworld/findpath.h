#pragma once
#include <misc/point.h>

namespace FAWorld
{
    class GameLevelImpl;
    class Actor;

    Misc::Points neighbors(GameLevelImpl* level, const Actor* actor, const Misc::Point& location);
    Misc::Points pathFind(GameLevelImpl* level, const Actor* actor, const Misc::Point& start, const Misc::Point& goal, bool& bArrivable, bool findAdjacent);
}
