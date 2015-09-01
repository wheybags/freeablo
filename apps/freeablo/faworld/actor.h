#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"
#include "inventory.h"

#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"

#include <misc/misc.h>

namespace FAWorld
{
    class ActorStats;
    class World;

    namespace AnimState
    {
        enum AnimState
        {
            walk,
            idle,
            dead
        };
    }

    class Actor
    {
        public:
            Actor(const std::string& walkAnimPath,
                  const std::string& idleAnimPath,
                  const Position& pos,
                  const std::string& dieAnimPath="",
                  ActorStats* stats=nullptr,
                  const std::string& soundPath="");
            void update(bool noclip);

            void setStats(ActorStats* stats);

            virtual ~Actor() = default;

            virtual std::string getDieWav(){return "";}

            virtual void takeDamage(double amount);

            virtual uint32_t getCurrentHP();

            bool isAttacking;

            FARender::FASpriteGroup getCurrentAnim();
            void setAnimation(AnimState::AnimState state);
            void setWalkAnimation(const std::string path);
            void setIdleAnimation(const std::string path);

            virtual bool attack(Actor * enemy)
            {
                UNUSED_PARAM(enemy);
               return false;
            }
            Position mPos;            
        //private: //TODO: fix this
            FARender::FASpriteGroup mWalkAnim;
            FARender::FASpriteGroup mIdleAnim;
            FARender::FASpriteGroup mDieAnim;

            size_t mFrame;
            Inventory mInventory;
            virtual void die();


            std::pair<size_t, size_t> mDestination;

            std::pair<size_t, size_t>& destination()
            {
                return mDestination;
            }
            bool isDead();

        protected:
            std::string mSoundPath;
            bool mIsDead;
            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                UNUSED_PARAM(version);

                ar & this->mPos;
                ar & this->mFrame;
                ar & this->mAnimState;
                ar & this->mDestination;
            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                UNUSED_PARAM(version);

                ar & this->mPos;
                ar & this->mFrame;
                ar & this->mAnimState;
                ar & this->mDestination;
            }

            BOOST_SERIALIZATION_SPLIT_MEMBER()
            ActorStats * mStats=nullptr;
        private:
            friend class Inventory;

            AnimState::AnimState mAnimState;

    };
}

#endif
