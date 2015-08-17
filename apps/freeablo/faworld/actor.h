#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"
#include "inventory.h"
#include "actorstats.hpp"
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
            Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos, ActorStats * stats=NULL);
            void update();

            FARender::FASpriteGroup getCurrentAnim();
            void setAnimation(AnimState::AnimState state);

            Position mPos;            
        //private: //TODO: fix this
            FARender::FASpriteGroup mWalkAnim;
            FARender::FASpriteGroup mIdleAnim;
            size_t mFrame;
            Inventory mInventory;



        private:
            friend class Inventory;
            ActorStats * mStats;
            AnimState::AnimState mAnimState;

    };
}

#endif
