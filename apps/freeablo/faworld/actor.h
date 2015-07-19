#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"
#include "inventory.h"
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
            Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos);
            void update();

            FARender::FASpriteGroup getCurrentAnim();
            void setAnimation(AnimState::AnimState state);

            Position mPos;
            Inventory inventory;
        //private: //TODO: fix this
            FARender::FASpriteGroup mWalkAnim;
            FARender::FASpriteGroup mIdleAnim;
            size_t mFrame;


        private:
            AnimState::AnimState mAnimState;

    };
}

#endif
