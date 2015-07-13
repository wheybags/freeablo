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
        mAnimState(AnimState::idle)
    {
        inventory = inventory.testInv();

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
        mAnimState = state;
        mFrame = 0;
    }
}
