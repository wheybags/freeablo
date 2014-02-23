#include "actor.h"

namespace FAWorld
{
    void Actor::update()
    {
        mPos.update(); 
    }
    
    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        switch(mAnimState)
        {
            case AnimState::walk:
                return mWalkAnim;

            case AnimState::idle:
                return mIdleAnim;
        }
    }
    
    void Actor::setAnimation(AnimState::AnimState state)
    {
        mAnimState = state;
        mFrame = 0;
    }
}
