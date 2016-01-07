#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"

#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"
#include <boost/format.hpp>
#include <misc/misc.h>
#include <map>

namespace Engine
{
    class NetManager;
}

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
            attack,
            dead,
            hit
        };
    }    
    class Actor : public NetObject
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

        public:
            Actor(const std::string& walkAnimPath="",
                  const std::string& idleAnimPath="",
                  const Position& pos = Position(0,0),
                  const std::string& dieAnimPath="",
                  ActorStats* stats=nullptr);

            void update(bool noclip, size_t ticksPassed);
            void setStats(ActorStats* stats);
            virtual ~Actor();
            virtual std::string getDieWav(){return "";}
            virtual std::string getHitWav(){return "";}
            virtual void setSpriteClass(std::string className){UNUSED_PARAM(className);}
            virtual void takeDamage(double amount);
            virtual int32_t getCurrentHP();
            bool mAnimPlaying = false;
            bool isAttacking = false;
            bool isTalking = false;
            virtual FARender::FASpriteGroup* getCurrentAnim();
            void setAnimation(AnimState::AnimState state, bool reset=false);
            void setWalkAnimation(const std::string path);
            void setIdleAnimation(const std::string path);            
            AnimState::AnimState getAnimState();

            int32_t getId()
            {
                return mId;
            }

            virtual void setLevel(GameLevel* level);
            GameLevel* getLevel();

            virtual bool attack(Actor * enemy)
            {
                UNUSED_PARAM(enemy);
                return false;
            }

            virtual bool talk(Actor * actor)
            {
                UNUSED_PARAM(actor);
                return false;
            }

            Position mPos;            
        //private: //TODO: fix this
            FARender::FASpriteGroup* mWalkAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mIdleAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mDieAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mAttackAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mHitAnim = FARender::getDefaultSprite();
        
            size_t mFrame;
            virtual void die();
            std::pair<int32_t, int32_t> mDestination;
            std::pair<int32_t, int32_t>& destination()
            {
                return mDestination;
            }

            bool canTalk() const;
            bool isDead() const;
            bool isEnemy() const;
            std::string getActorId() const;
            void setActorId(const std::string& id);
            void setCanTalk(bool canTalk);

            std::map<AnimState::AnimState, size_t> mAnimTimeMap;
            ActorStats * mStats=nullptr;
            virtual size_t getWriteSize();
            virtual bool writeTo(ENetPacket *packet, size_t& position);
            virtual bool readFrom(ENetPacket *packet, size_t& position);

        protected:
            GameLevel* mLevel = NULL;

            bool mIsDead = false;
            bool mCanTalk = false;
            bool mIsEnemy;

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

            bool canIAttack(Actor * actor);
            bool canTalkTo(Actor * actor);

            BOOST_SERIALIZATION_SPLIT_MEMBER()
            AnimState::AnimState mAnimState;

        private:
            std::string mActorId;
            int32_t mId;
            friend class Engine::NetManager;
    };
}

#endif
