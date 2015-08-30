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
    bool Actor::attack(Actor *enemy)
    {
        if(enemy->mStats != NULL && enemy != this)
        {
            if(mPos.distanceFrom(enemy->mPos) <= 2)
            {
                    //setAnimation(AnimState::attackMelee);
                    Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne("sfx/misc/swing2.wav", "sfx/misc/swing.wav"));
                    enemy->mStats->takeDamage(mStats->getMeleeDamage());
                    if(enemy->mStats->getCurrentHP() ==0)
                        enemy->die();
                    return true;

            }
            else
                return false;
        }
        else
            return false;
        return false;


    }

    void Actor::setWorld(World *world)
    {
        mWorld = world;
    }


    Actor::Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos):
        mPos(pos),
        mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)),
        mIdleAnim(FARender::Renderer::get()->loadImage(idleAnimPath)),
        mFrame(0),
        mInventory(this),        
        mAnimState(AnimState::idle)
    {
        mDestination = mPos.current();
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
        mWorld->deleteActorFromWorld(this);
    }


    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        switch(mAnimState)
        {
            case AnimState::walk:
                return mWalkAnim;

            default: // AnimState::idle:
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
