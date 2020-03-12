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
        int32_t tmpX = abs(a.current().x - b.current().x);
        int32_t tmpY = abs(a.current().y - b.current().y);
        return tmpX * tmpX + tmpY * tmpY;
    }

    // TODO: could be a method on Actor class
    Player* findNearestPlayer(const Actor* actor)
    {
        Player* nearest = nullptr;
        int minDistance = 99999999;
        for (auto player : actor->getWorld()->getPlayers())
        {
            if (player->isDead())
                continue;

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

    void BasicMonsterBehaviour::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("BasicMonsterBehaviour", saver);

        saver.save(mTicksSinceLastAction);
    }

    void BasicMonsterBehaviour::update()
    {
        if (mActor->mTarget.getType() != Target::Type::None)
            return;

        mTicksSinceLastAction++;

        if (!mActor->isDead())
        {
            Player* nearest = FAWorld::findNearestPlayer(mActor);

            int32_t dist = 0;

            if (nearest)
                dist = FAWorld::squaredDistance(nearest->getPos(), mActor->getPos());

            if (nearest && dist <= std::pow(5, 2)) // we are close enough to engage the player
            {
                if (mTicksSinceLastAction >= World::getTicksInPeriod("1"))
                {
                    mActor->mTarget = nearest;
                    mTicksSinceLastAction = 0;
                }
                else
                {
                    mActor->mTarget = mActor->getPos().current();
                }
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
                    Misc::Point next;

                    int its = 0;
                    do
                    {
                        ++its;
                        next = mActor->getPos().current();

                        next.x += mActor->getWorld()->mRng->randomInRange(-5, 5);
                        next.y += mActor->getWorld()->mRng->randomInRange(-5, 5);
                    } while (its < 10 && (!mActor->getLevel()->isPassable(next, mActor) || next == mActor->getPos().current()));

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
