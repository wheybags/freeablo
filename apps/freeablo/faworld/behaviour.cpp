#include "behaviour.h"
#include "actor.h"
#include "player.h"

#include <iostream>

#include <stdlib.h>

namespace FAWorld
{

    static int squaredDistance(const Position& a, const Position& b)
    {
        int tmpX = abs(a.current().first - b.current().first);
        int tmpY = abs(a.current().second - b.current().second);
        return tmpX*tmpX + tmpY*tmpY;
    }

    // TODO: could be a method on Actor class
    const Player* findNearestPlayer(const Actor* actor)
    {
        Player* nearest;
        int minDistance = 99999999;
        for (auto player : World::get()->getPlayers())
        {
            int distance = FAWorld::squaredDistance(player->mPos, actor->mPos);
            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = player;
            }
        }

        return nearest;
    }

    void BasicMonsterBehaviour::update(size_t ticksPassed)
    {
        if (!mActor->isDead()) {
            const Player * nearest = FAWorld::findNearestPlayer(mActor);
            // we are close enough to engage the player
            if (FAWorld::squaredDistance(nearest->mPos, mActor->mPos) <= 25) {
                mActor->mDestination = mActor->mPos.mGoal = nearest->mPos.current();
            }
            // if no player is in sight, let's wander around a bit
            else if (!mActor->mPos.mMoving) {
                // we arrived at the destination, so let's decide if
                // we want to move some more
                if (ticksPassed - mLastActionTick > ((size_t)rand() % 300) + 100) {
                    // if 1 let's move, else wbehaviourt
                    if (rand() % 2) {
                        auto next = mActor->mPos.current();
                        do {
                            next = mActor->mPos.current();
                            next.first += ((rand() % 3) - 1) * (rand() % 3 + 1);
                            next.second += ((rand() % 3) - 1) * (rand() % 3 + 1);
                        } while (!mActor->canWalkTo(next.first, next.second));
                        mActor->mDestination = mActor->mPos.mGoal = next;
                    }
                    mLastActionTick = ticksPassed;
                }
            }
        }
    }

}
