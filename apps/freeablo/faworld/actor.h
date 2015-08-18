#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"
#include "inventory.h"
#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"

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
            Inventory mInventory;
        //private: //TODO: fix this
            FARender::FASpriteGroup mWalkAnim;
            FARender::FASpriteGroup mIdleAnim;
            size_t mFrame;

        protected:

            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                ar & this->mPos;
            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                ar & this->mPos;
                mAnimState = AnimState::idle;
            }

            BOOST_SERIALIZATION_SPLIT_MEMBER()

        private:
            AnimState::AnimState mAnimState;

    };
}

#endif
