#include <shadowcaster/shadowcaster.h>
#include "actorvision.h"
#include "actor.h"

namespace FAWorld
{

    ActorVision::ActorVision(const Actor* actor): mActor(actor), mOrigin(actor->mPos.current()) {}

    void ActorVision::update()
    {
        auto level = mActor->getLevel();
        auto start = mActor->mPos.current();
        if (start != mOrigin) {
            mOrigin = start;
            ShadowCaster::Scanner scanner(level, start, 8);

            mVisibleTiles = scanner.getVisibleTiles();
            mVisibleTiles.insert(start);
        }
    }

}
