#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"
#include "inventory.h"

#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"

namespace FAWorld
{
    class ActorStats;

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
            void update(bool noclip);

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


            std::pair<size_t, size_t> mDestination;

            std::pair<size_t, size_t>& destination()
            {
                return mDestination;
            }

        protected:

            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                ar & this->mPos;
                ar & this->mFrame;
                ar & this->mAnimState;
                ar & this->mDestination;
            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                ar & this->mPos;
                ar & this->mFrame;
                ar & this->mAnimState;
                ar & this->mDestination;
            }

            BOOST_SERIALIZATION_SPLIT_MEMBER()

        private:
            friend class Inventory;
            ActorStats * mStats = NULL;
            AnimState::AnimState mAnimState;

    };
}

#endif
