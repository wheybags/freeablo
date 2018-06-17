#include "behaviour.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include "player.h"
#include <cstdlib>
#include <iostream>
#include <misc/assert.h>
#include <random/random.h>

namespace FAWorld
{
    const std::string BasicMonsterBehaviour::typeId = "basic-monster-behaviour";
    const std::string NullBehaviour::typeId = "null-behaviour";

    static int32_t squaredDistance(const Position& a, const Position& b)
    {
        int32_t tmpX = abs(a.current().first - b.current().first);
        int32_t tmpY = abs(a.current().second - b.current().second);
        return tmpX * tmpX + tmpY * tmpY;
    }

    // TODO: could be a method on Actor class
    Player* findNearestPlayer(const Actor* actor)
    {
        Player* nearest = nullptr;
        int minDistance = 99999999;
        for (auto player : actor->getWorld()->getPlayers())
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

    BasicMonsterBehaviour::BasicMonsterBehaviour(FASaveGame::GameLoader& loader) { mTicksSinceLastAction = loader.load<Tick>(); }

    void BasicMonsterBehaviour::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("BasicMonsterBehaviour", saver);

        saver.save(mTicksSinceLastAction);
    }

    void BasicMonsterBehaviour::update()
    {
        mTicksSinceLastAction++;

        if (!mActor->isDead())
        {
            Player* nearest = FAWorld::findNearestPlayer(mActor);

            int32_t dist = FAWorld::squaredDistance(nearest->getPos(), mActor->getPos());

            if (dist <= std::pow(5, 2)) // we are close enough to engage the player
            {
                mActor->mTarget = nearest;
            }
            else if (dist >= std::pow(100, 2)) // just freeze if we're miles away from anyone
            {
                return;
            }
            // if no player is in sight, let's wander around a bit
            else if (mTicksSinceLastAction > World::getTicksInPeriod("0.5") && !mActor->hasTarget() && !mActor->mMoveHandler.moving())
            {
                if (mActor->getWorld()->mRng->randomInRange(0, 100) > 80)
                {
                    std::pair<int32_t, int32_t> next;

                    int its = 0;
                    do
                    {
                        ++its;
                        next = mActor->getPos().current();

                        next.first += mActor->getWorld()->mRng->randomInRange(-5, 5);
                        next.second += mActor->getWorld()->mRng->randomInRange(-5, 5);
                    } while (its < 10 && (!mActor->getLevel()->isPassable(next.first, next.second) || next == mActor->getPos().current()));

                    static int no = 0;
                    static int yes = 0;

                    if (its < 10)
                    {
                        mActor->mMoveHandler.setDestination(next);
                        yes++;
                    }
                    else
                        no++;

                    mTicksSinceLastAction = 0;
                }
            }
        }
    }
}
