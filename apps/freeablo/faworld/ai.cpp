#include "ai.h"

namespace FAWorld
{

    static int squaredDistance(Position a, Position b)
    {
        int tmpX = abs(a.current().first - b.current().first);
        int tmpY = abs(a.current().second - b.current().second);
        return tmpX*tmpX + tmpY*tmpY;
    }

    void BasicMonsterAI::update()
    {
        if (!mActor->isDead()) {
            const Player * nearest = findNearestPlayer();
            // we are close enough to engage the player
            if (FAWorld::squaredDistance(nearest->mPos, mActor->mPos) <= 25) {
                mActor->mDestination = mActor->mPos.mGoal = nearest->mPos.current();
            }
        }
    }

    const Player * BasicMonsterAI::findNearestPlayer()
    {
        Player * nearest;
        int minDistance = 99999999;
        for (auto player : World::get()->getPlayers())
        {
            int distance = FAWorld::squaredDistance(player->mPos, mActor->mPos);
            if (distance < minDistance)
            {
                minDistance = distance;
                nearest = player;
            }
        }

        return nearest;
    }

}
