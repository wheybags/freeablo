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

namespace Random
{
    class Rng;
}

namespace FALevelGen
{
    class TileSet;
    Level::Dun generateBasic(Random::Rng& rng, TileSet& tileset, int32_t width, int32_t height, int32_t levelNum);

    FAWorld::GameLevel* generate(
        FAWorld::World& world, Random::Rng& rng, int32_t width, int32_t height, int32_t dLvl, const DiabloExe::DiabloExe& exe, int32_t previous, int32_t next);
}
