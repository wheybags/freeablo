#include "actor.h"

#include <misc/misc.h>
#include "actorstats.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../falevelgen/random.h"
namespace FAWorld
{

    void Actor::update(bool noclip)
    {        
        if(mPos.current() != mDestination)
        {
            Actor * enemy;
            enemy = World::get()->getActorAt(mDestination.first, mDestination.second);
            if (enemy != nullptr && mPos.distanceFrom(enemy->mPos) < 2 && this != enemy && !isAttacking)
                    attack(enemy);
            if(mPos.mDist == 0)
            {
                std::pair<float, float> vector = Misc::getVec(mPos.current(), mDestination);
                auto nextPos = Position(mPos.current().first, mPos.current().second, Misc::getVecDir(vector));
                nextPos.mMoving = true;
                World& world = *World::get();
                FAWorld::Actor* actorAtNext = world.getActorAt(nextPos.next().first, nextPos.next().second);

                if(noclip || ((*world.getCurrentLevel())[nextPos.next().first][nextPos.next().second].passable() &&
                                                   (actorAtNext == NULL || actorAtNext == this)))
                {
                    if(!mPos.mMoving)
                    {
                        mPos.mMoving = true;
                        setAnimation(FAWorld::AnimState::walk);
                    }
                }
                else
                {
                    mPos.mMoving = false;
                    mDestination = mPos.current();
                    setAnimation(FAWorld::AnimState::idle);
                }

                mPos.mDirection = Misc::getVecDir(vector);
            }
        }
        else if(mPos.mMoving && mPos.mDist == 0)
        {
            mPos.mMoving = false;
            setAnimation(FAWorld::AnimState::idle);
        }

        mPos.update(); 
    }


    Actor::Actor(
            const std::string& walkAnimPath,
            const std::string& idleAnimPath,
            const Position& pos,
            const std::string& dieAnimPath,
            ActorStats* stats,
            const std::string& soundPath):
        mPos(pos),
        mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)),
        mIdleAnim(FARender::Renderer::get()->loadImage(idleAnimPath)),
        mFrame(0),
        mInventory(this),
        mSoundPath(soundPath),
        mStats(stats),
        mAnimState(AnimState::idle)
    {
        if (!dieAnimPath.empty())
        {
            mDieAnim = FARender::Renderer::get()->loadImage(dieAnimPath);
        }
        mDestination = mPos.current();
    }

    void Actor::takeDamage(double amount)
    {
        mStats->takeDamage(amount);

    }

    uint32_t Actor::getCurrentHP()
    {
        return mStats->getCurrentHP();
    }

    void Actor::setWalkAnimation(const std::string path)
    {
        mWalkAnim = FARender::Renderer::get()->loadImage(path);
    }

    void Actor::setIdleAnimation(const std::string path)
    {
        mIdleAnim = FARender::Renderer::get()->loadImage(path);
    }

    void Actor::die()
    {
        setAnimation(AnimState::dead);
        mIsDead = true;
        Engine::ThreadManager::get()->playSound(getDieWav());
        //World::get()->deleteActorFromWorld(this);
    }

    bool Actor::isDead()
    {
        return mIsDead;
    }

    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        switch(mAnimState)
        {
            case AnimState::walk:
                return mWalkAnim;

            case AnimState::idle:
                return mIdleAnim;

            case AnimState::dead:
                return mDieAnim;

            default:
                return mIdleAnim;

        }
    }
    void Actor::setStats(ActorStats * stats)
    {
        mStats = stats;

    }

    void Actor::setAnimation(AnimState::AnimState state)
    {
        if(mAnimState != state)
        {
            mAnimState = state;
            mFrame = 0;
        }
    }
}
