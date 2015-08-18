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
            Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos);
            void update();

            void setStats(ActorStats* stats);

            FARender::FASpriteGroup getCurrentAnim();
            void setAnimation(AnimState::AnimState state);
            void setWalkAnimation(const std::string path);
            void setIdleAnimation(const std::string path);

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
