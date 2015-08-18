#include "actor.h"

namespace FAWorld
{
    void Actor::update()
    {
        mPos.update(); 
    }
    Actor::Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos):
        mPos(pos),
        mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)),
        mIdleAnim(FARender::Renderer::get()->loadImage(idleAnimPath)),
        mFrame(0),
        mInventory(this),        
        mAnimState(AnimState::idle)
    {
    }

    void Actor::setWalkAnimation(const std::string path)
    {
        mWalkAnim = FARender::Renderer::get()->loadImage(path);
    }

    void Actor::setIdleAnimation(const std::string path)
    {
        mIdleAnim = FARender::Renderer::get()->loadImage(path);
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
        mAnimState = state;
        mFrame = 0;
    }
}
