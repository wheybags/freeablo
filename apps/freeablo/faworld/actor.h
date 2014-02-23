#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"

#include "../farender/renderer.h"

namespace FAWorld
{
    namespace AnimState
    {
        enum AnimState
        {
            walk,
            idle
        };
    }

    class Actor
    {
        public:
            Actor(const std::string& walkAnimPath, const std::string& idleAnimPath):
                mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)), 
                mIdleAnim(FARender::Renderer::get()->loadImage(idleAnimPath)), 
                mAnimState(AnimState::idle), 
                mFrame(0) {}
            void update();

            FARender::FASpriteGroup getCurrentAnim();
            void setAnimation(AnimState::AnimState state);

            Position mPos;
        //private: //TODO: fix this
            FARender::FASpriteGroup mWalkAnim;
            FARender::FASpriteGroup mIdleAnim;
            size_t mFrame;

        private:
            AnimState::AnimState mAnimState;
    };
}

#endif
