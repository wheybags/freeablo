#include "actor.h"

#include <misc/misc.h>

#include "world.h"

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
    Actor::Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos):
        mPos(pos),
        mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)),
        mIdleAnim(FARender::Renderer::get()->loadImage(idleAnimPath)),
        mFrame(0),
        mAnimState(AnimState::idle)
    {
        mInventory = mInventory.testInv(mInventory);
        mDestination = mPos.current();
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
    
    void Actor::setAnimation(AnimState::AnimState state)
    {
        if(mAnimState != state)
        {
            mAnimState = state;
            mFrame = 0;
        }
    }
}
