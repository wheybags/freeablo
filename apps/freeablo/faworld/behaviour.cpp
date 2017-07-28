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
            Tick ticksPassed = World::get()->getCurrentTick();

            Player * nearest = FAWorld::findNearestPlayer(mActor);

            int32_t dist = FAWorld::squaredDistance(nearest->getPos(), mActor->getPos());

            if (dist <= std::pow(5, 2)) // we are close enough to engage the player
            {
                mActor->actorTarget = nearest;
            }
            else if (dist >= std::pow(100, 2)) // just freeze if we're miles away from anyone
            {
                //mActor->die();
                //mActor->actorTarget = nullptr;
                return;
            }
            else if (mActor->actorTarget == nullptr && !mActor->mMoveHandler.moving()) // if no player is in sight, let's wander around a bit
            {
                // seed a simple RNG with some variables that should be stable across server and client
                FALevelGen::RandLCG r(mTicksSinceLastAction + mActor->getId() + mActor->getPos().current().first);

                // we arrived at the destination, so let's decide if
                // we want to move some more
                if (mTicksSinceLastAction > ((size_t)r.get() % 300) + 100)
                {
                    // if 1 let's move, else wbehaviourt
                    if (r.get() % 2) 
                    {
                        std::pair<int32_t, int32_t> next;

                        do 
                        {
                            next = mActor->getPos().current();
                            next.first += ((r.get() % 3) - 1) * (r.get() % 3 + 1);
                            next.second += ((r.get() % 3) - 1) * (r.get() % 3 + 1);
                        } while (!mActor->getLevel()->isPassable(next.first, next.second) || next == mActor->getPos().current());
                        
                        mActor->mMoveHandler.setDestination(next);
                    }

                    mTicksSinceLastAction = 0;
                }
            }
        }
    }
}
