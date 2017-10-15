#ifndef ACTOR_H
#define ACTOR_H

#include <map>

#include <boost/format.hpp>

#include <statemachine/statemachine.h>
#include <misc/misc.h>

#include "../engine/net/netmanager.h"
#include "../farender/animationplayer.h"
#include "../fasavegame/savegame.h"

#include "position.h"
#include "gamelevel.h"
#include "world.h"
#include "faction.h"
#include "movementhandler.h"
#include "actoranimationmanager.h"
#include "behaviour.h"
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>


namespace Engine
{
    class NetManager;
    class Server;
    class Client;
}

namespace FAWorld
{
    class ActorStats;
    class Behaviour;
    class World;
    class ItemTarget;

    namespace ActorState
    {
        class BaseState;
    }

    class Actor : public NetObject
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()
        void interact(Actor* actor);
        void setIdleAnimSequence(const std::vector<int> & sequence);
        friend class ActorState::BaseState; // TODO: fix

        public:
            Actor(const std::string& walkAnimPath="",
                  const std::string& idleAnimPath="",
                  const std::string& dieAnimPath=""
                  );

            virtual void update(bool noclip);
            virtual ~Actor();
            virtual std::string getDieWav(){return "";}
            virtual std::string getHitWav(){return "";}
            virtual void setSpriteClass(std::string className){UNUSED_PARAM(className);}
            virtual void takeDamage(double amount);
            virtual int32_t getCurrentHP();
            virtual void pickupItem(ItemTarget /*target*/) {}
            bool hasTarget() const;

            bool isAttacking = false;
            bool isTalking = false;

            StateMachine::StateMachine<Actor>* mActorStateMachine;

            ActorAnimationManager& getAnimationManager()
            {
                return mAnimation;
            }

            bool isPassable()
            {
                return mPassable || mIsDead;
            }

            void setPassable(bool passable)
            {
                mPassable = passable;
            }

            int32_t getId()
            {
                return mId;
            }

            std::string getName() const;
            void setName(const std::string& name);

            void teleport(GameLevel* level, Position pos);
            GameLevel* getLevel();

            bool attack(Actor * enemy);

            virtual bool talk(Actor * actor)
            {
                UNUSED_PARAM(actor);
                return false;
            }

            using TargetType = boost::variant<boost::blank, Actor*, ItemTarget>;
            void setTarget (TargetType target);

            MovementHandler mMoveHandler;
            TargetType mTarget;

            Position getPos() const
            {
                return mMoveHandler.getCurrentPosition();
            }

            void setInvuln(bool invuln)
            {
                mInvuln = invuln;
            }

        //private: //TODO: fix this


            virtual void die();

            bool canTalk() const;
            bool isDead() const;
            bool isEnemy(Actor* other) const;
            std::string getActorId() const;
            void setActorId(const std::string& id);
            void setCanTalk(bool canTalk);

            bool canWalkTo(int32_t x, int32_t y);

            void attachBehaviour(Behaviour* behaviour) {
              mBehaviour = behaviour;
            };

            ActorStats * mStats=nullptr;

            template <class Stream>
            Serial::Error::Error faSerial(Stream& stream)
            {
                auto destTmp = mMoveHandler.getDestination();
                auto levelTmp = mMoveHandler.getLevel();
                serialise_object(stream, mMoveHandler);


                if (!stream.isWriting())
                {
                    // make sure we let the level object know if we've changed levels
                    if (levelTmp != mMoveHandler.getLevel())
                    {
                        if (levelTmp)
                            levelTmp->removeActor(this);

                        mMoveHandler.getLevel()->addActor(this);

                        destTmp = mMoveHandler.getCurrentPosition().current(); // just sit still after a level change
                    }


                    // don't sync our own destination, we set that ourselves
                    if ((Actor*)World::get()->getCurrentPlayer() == this)
                        mMoveHandler.setDestination(destTmp);
                }

                serialise_object(stream, mAnimation);

                serialise_bool(stream, mIsDead);

                if(mStats)
                    serialise_object(stream, *mStats);

                bool hasBehaviour = mBehaviour != nullptr;

                serialise_bool(stream, hasBehaviour);

                if (hasBehaviour)
                {
                    int32_t classId = -1;

                    if(stream.isWriting())
                        classId = mBehaviour->getClassId();

                    serialise_int32(stream, classId);

                    if (!stream.isWriting() && mBehaviour == nullptr)
                    {
                        auto behaviour = (Behaviour*)NetObject::construct(classId);
                        behaviour->attach(this);
                        attachBehaviour(behaviour);
                    }

                    Serial::Error::Error err = mBehaviour->streamHandle(stream);
                    if (err != Serial::Error::Success)
                        return err;
                }

                int32_t targetId = -1;
                if (stream.isWriting() && mTarget.type() == typeid (Actor *))
                    targetId = boost::get<Actor *> (mTarget)->getId ();

                serialise_int32(stream, targetId);

                if (!stream.isWriting() && targetId != -1)
                    setTarget (World::get()->getActorById(targetId));

                return Serial::Error::Success;
            }

        protected:
            Behaviour* mBehaviour = nullptr;

            bool mIsDead = false;
            bool mCanTalk = false;
            Faction mFaction;

            bool mPassable = false;

            ActorAnimationManager mAnimation;

            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                UNUSED_PARAM(version);
                UNUSED_PARAM(ar);
                assert(false);
                //ar & this->mPos;
                //ar & this->mFrame;
                //ar & this->mDestination;
            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                UNUSED_PARAM(version);
                UNUSED_PARAM(ar);
                assert(false);
                //ar & this->mPos;
                //ar & this->mFrame;
                //ar & this->mDestination;
            }

            virtual bool canIAttack(Actor * actor);
            bool canInteractWith(Actor* actor);
            bool canTalkTo(Actor * actor);

            BOOST_SERIALIZATION_SPLIT_MEMBER()

            bool mInvuln = false;

        private:
            std::string mActorId;
            std::string mName;
            int32_t mId;
            friend class Engine::Server; // TODO: fix
            friend class Engine::Client;
            friend class Engine::NetManager;
    };
}

#endif
