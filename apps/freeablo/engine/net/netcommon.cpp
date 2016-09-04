#include "netcommon.h"

#include "../../faworld/world.h"

namespace Engine
{
    uint32_t getStallThreshold()
    {
        return FAWorld::World::ticksPerSecond * 3;
    }
}