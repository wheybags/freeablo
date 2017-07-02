#ifndef ACTOR_H
#define ACTOR_H

#include <map>

#include <boost/format.hpp>

#include <misc/misc.h>

#include "../engine/net/netmanager.h"
#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"

#include "position.h"
#include "gamelevel.h"
#include "world.h"
#include "faction.h"
#include "actorvision.h"

namespace Engine
{
    class NetManager;
    class Server;
    class Client;
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
            hit,
            ENUM_END // always leave this as the last entry, and don't set explicit values for any of the entries
        };
    }
    class Actor : public NetObject
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

        public:
            Actor(const std::string& walkAnimPath="",
                  const std::string& idleAnimPath="",
                  const Position& pos = Position(0,0),
                  const std::string& dieAnimPath=""
                  );

            virtual void update(bool noclip, size_t ticksPassed);
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
            bool findPath(GameLevelImpl* level, std::pair<int32_t, int32_t> destination);


            int32_t getId()
            {
                return mId;
            }

            virtual void setLevel(GameLevel* level);
            GameLevel* getLevel() const;

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
            bool isEnemy(Actor* other) const;
            std::string getActorId() const;
            void setActorId(const std::string& id);
            void setCanTalk(bool canTalk);

            std::map<AnimState::AnimState, size_t> mAnimTimeMap;
            ActorStats * mStats=nullptr;

            template <class Stream>
            Serial::Error::Error faSerial(Stream& stream)
            {
                serialise_object(stream, mPos);
                serialise_int(stream, 0, 2048, mFrame);
                serialise_enum(stream, AnimState::AnimState, mAnimState);

                int32_t destXTmp = mDestination.first;
                int32_t destYTmp = mDestination.second;
                int32_t levelIndexTmp = -1;
                if (mLevel)
                    levelIndexTmp = mLevel->getLevelIndex();

                serialise_int32(stream, destXTmp);
                serialise_int32(stream, destYTmp);
                serialise_int32(stream, levelIndexTmp);

                if (!stream.isWriting())
                {
                    if ((Actor*)World::get()->getCurrentPlayer() != this)
                    {
                        // don't want to read destination for our player object,
                        // we keep track of our own destination
                        mDestination.first = destXTmp;
                        mDestination.second = destYTmp;

                        if (levelIndexTmp != -1)
                            setLevel(World::get()->getLevel(levelIndexTmp));
                    }
                }

                uint32_t walkAnimIndex = 0;
                uint32_t idleAnimIndex = 0;
                uint32_t dieAnimIndex = 0;
                uint32_t attackAnimIndex = 0;
                uint32_t hitAnimIndex = 0;

                if (stream.isWriting())
                {
                    if (mWalkAnim)
                        walkAnimIndex = mWalkAnim->getCacheIndex();
                    if (mIdleAnim)
                        idleAnimIndex = mIdleAnim->getCacheIndex();
                    if (mDieAnim)
                        dieAnimIndex = mDieAnim->getCacheIndex();
                    if (mAttackAnim)
                        attackAnimIndex = mAttackAnim->getCacheIndex();
                    if (mHitAnim)
                        hitAnimIndex = mHitAnim->getCacheIndex();
                }

                serialise_int32(stream, walkAnimIndex);
                serialise_int32(stream, idleAnimIndex);
                serialise_int32(stream, dieAnimIndex);
                serialise_int32(stream, attackAnimIndex);
                serialise_int32(stream, hitAnimIndex);

                if (!stream.isWriting())
                {
                    mWalkAnim = FARender::getDefaultSprite();
                    mIdleAnim = FARender::getDefaultSprite();
                    mDieAnim = FARender::getDefaultSprite();
                    mAttackAnim = FARender::getDefaultSprite();
                    mHitAnim = FARender::getDefaultSprite();

                    auto netManager = Engine::NetManager::get();

                    if (walkAnimIndex)
                        mWalkAnim = netManager->getServerSprite(walkAnimIndex);
                    if (idleAnimIndex)
                        mIdleAnim = netManager->getServerSprite(idleAnimIndex);
                    if (dieAnimIndex)
                        mDieAnim = netManager->getServerSprite(dieAnimIndex);
                    if (attackAnimIndex)
                        mAttackAnim = netManager->getServerSprite(attackAnimIndex);
                    if (hitAnimIndex)
                        mHitAnim = netManager->getServerSprite(hitAnimIndex);
                }

                serialise_bool(stream, mIsDead);

                if(mStats)
                    serialise_object(stream, *mStats);

                return Serial::Error::Success;
            }

            inline bool canSee(const std::pair<int32_t, int32_t>& pos) const
            {
                return mVision.canSee(pos.first, pos.second);
            }

        protected:
            GameLevel* mLevel = NULL;
            ActorVision mVision;

            bool mIsDead = false;
            bool mCanTalk = false;
            Faction mFaction;

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
            friend class Engine::Server; // TODO: fix
            friend class Engine::Client;
            friend class Engine::NetManager;
    };
}

#endif
