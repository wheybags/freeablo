#include <shadowcaster/shadowcaster.h>
#include "actorvision.h"
#include "actor.h"

namespace FAWorld
{

    void ActorVision::update()
    {
        auto level = mActor->getLevel();
        auto start = mActor->mPos.current();
        ShadowCaster::Scanner scanner(level, start, 8);

        mVisibleTiles = scanner.getVisibleTiles();
        mVisibleTiles.insert(start);
    }

}
