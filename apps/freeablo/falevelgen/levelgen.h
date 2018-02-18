
#pragma once

#include "../faworld/gamelevel.h"

namespace DiabloExe
{
    class DiabloExe;
}

namespace FAWorld
{
    class World;
}

namespace FALevelGen
{

    FAWorld::GameLevel*
    generate(FAWorld::World& world, int32_t width, int32_t height, int32_t dLvl, const DiabloExe::DiabloExe& exe, int32_t previous, int32_t next);
}
