#include "behaviour.h"
#include "actor.h"
#include "player.h"
#include "../falevelgen/random.h"

#include <iostream>

#include <stdlib.h>

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(BasicMonsterBehaviour)
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(NullBehaviour)


    static int32_t squaredDistance(const Position& a, const Position& b)
    {
        int32_t tmpX = abs(a.current().first - b.current().first);
        int32_t tmpY = abs(a.current().second - b.current().second);
        return tmpX*tmpX + tmpY*tmpY;
    }

    // TODO: could be a method on Actor class
    Player* findNearestPlayer(const Actor* actor)
    {
        Player* nearest = nullptr;
        int minDistance = 99999999;
        for (auto player : World::get()->getPlayers())
        {
            int32_t distance = FAWorld::squaredDistance(player->getPos(), actor->getPos());
            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = player;
            }
        }

        return nearest;
    }

    void BasicMonsterBehaviour::update()
    {
        mTicksSinceLastAction++;

        if (!mActor->isDead())
        {
            Player * nearest = FAWorld::findNearestPlayer(mActor);

            int32_t dist = FAWorld::squaredDistance(nearest->getPos(), mActor->getPos());

            if (dist <= std::pow(5, 2)) // we are close enough to engage the player
            {
                mActor->setTarget (nearest);
            }
            else if (dist >= std::pow(100, 2)) // just freeze if we're miles away from anyone
            {
                return;
            }
            // if no player is in sight, let's wander around a bit
            else if (mTicksSinceLastAction > World::getTicksInPeriod(0.5f) && !mActor->hasTarget () && !mActor->mMoveHandler.moving())
            {
                // seed a simple RNG with some variables that should be stable across server and client
                FALevelGen::RandLCG r(mTicksSinceLastAction + mActor->getId() + mActor->getPos().current().first);

                if ((r.get() % 100) > 80)
                {
                    std::pair<int32_t, int32_t> next;

                    int its = 0;
                    do
                    {
                        ++its;
                        next = mActor->getPos().current();
                        next.first += ((r.get() % 3) - 1) * (r.get() % 3 + 1);
                        next.second += ((r.get() % 3) - 1) * (r.get() % 3 + 1);
                    } while (its < 10 && (!mActor->getLevel()->isPassable(next.first, next.second) || next == mActor->getPos().current()));

                    if(its < 10)
                        mActor->mMoveHandler.setDestination(next);

                    mTicksSinceLastAction = 0;
                }
            }
        }
    }
}
