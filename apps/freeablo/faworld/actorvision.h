#ifndef ACTORVISION_H
#define ACTORVISION_H

#include <map>

namespace FAWorld
{

    class ActorVision
    {
    public:
        ~ActorVision() {}

        const std::map<std::pair<int32_t, int32_t>, bool>& getVisibleTiles();

    };

}

#endif
