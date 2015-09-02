#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"
#include "inventory.h"

#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"
#include <boost/format.hpp>
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
            meleeAttack,
            dead,
            hit
        };
    }
    class ActorAnimState
    {
        public:
            ActorAnimState(){}
            ActorAnimState(const std::string &className, const std::string &armourCode, const std::string &weaponCode, bool inDungeon=false);
            void setWeapon(std::string weaponCode);
            void setArmour(std::string armourCode);
            void setClass(std::string className);
            void setDungeon(bool isDungeon);            
            std::string getStateString();
            std::string getAnimPath(AnimState::AnimState animState);
        private:
            void reconstructString();
            std::string mClassName;
            std::string mClassCode;
            std::string mArmourCode;
            std::string mWeaponCode;
            boost::format * mFmt;
            bool mInDungeon = true;
    };

    class Actor
    {
        public:
            Actor(const std::string& walkAnimPath="",
                  const std::string& idleAnimPath="",
                  const Position& pos = Position(0,0),
                  const std::string& dieAnimPath="",
                  ActorStats* stats=nullptr,
                  const std::string& soundPath="");
            void update(bool noclip);
            void setStats(ActorStats* stats);
            virtual ~Actor() = default;
            virtual std::string getDieWav(){return "";}
            virtual std::string getHitWav(){return "";}
            virtual void setSpriteClass(std::string className){UNUSED_PARAM(className);}
            virtual void takeDamage(double amount);
            virtual int32_t getCurrentHP();
            bool isAttacking;
            bool mAnimPlaying = false;
            double mAnimStep = 1;
            virtual FARender::FASpriteGroup getCurrentAnim();
            void setAnimation(AnimState::AnimState state, bool reset=false);
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
            double mFrame;
            Inventory mInventory;
            virtual void die();
            std::pair<size_t, size_t> mDestination;
            std::pair<size_t, size_t>& destination()
            {
                return mDestination;
            }
            bool isDead();
            ActorAnimState mActorSpriteState;

        protected:
            std::string mSoundPath;
            std::string mAnimPath;
            bool mIsDead = false;
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
            AnimState::AnimState mAnimState;
        private:
            friend class Inventory;
            friend class World;
    };
}

#endif
