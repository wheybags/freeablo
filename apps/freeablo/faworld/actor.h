#ifndef ACTOR_H
#define ACTOR_H

#include <map>

#include <boost/format.hpp>

#include <misc/misc.h>
#include <statemachine/statemachine.h>

#include "../farender/animationplayer.h"

#include "actoranimationmanager.h"
#include "actorstats.h"
#include "behaviour.h"
#include "faction.h"
#include "gamelevel.h"
#include "movementhandler.h"
#include "position.h"
#include "world.h"
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

namespace FASaveGame
{
    class Loader;
    class Saver;
}

namespace FAWorld
{
    class Behaviour;
    class World;
    class ItemTarget;

    namespace ActorState
    {
        class BaseState;
    }

    class Actor
    {
    public:
        void interact(Actor* actor);
        void setIdleAnimSequence(const std::vector<int32_t>& sequence);
        void setTalkData(const std::unordered_map<std::basic_string<char>, std::basic_string<char>>& talkData);
        friend class ActorState::BaseState; // TODO: fix

    public:
        Actor(const std::string& walkAnimPath = "", const std::string& idleAnimPath = "", const std::string& dieAnimPath = "");

        Actor(FASaveGame::GameLoader& loader);
        virtual void save(FASaveGame::GameSaver& saver);

        static const std::string typeId;
        virtual const std::string& getTypeId() { return typeId; }

        virtual void update(bool noclip);
        virtual ~Actor();
        virtual std::string getDieWav() { return ""; }
        virtual std::string getHitWav() { return ""; }
        virtual void setSpriteClass(std::string className) { UNUSED_PARAM(className); }
        virtual void takeDamage(double amount);
        virtual int32_t getCurrentHP();
        virtual void pickupItem(ItemTarget /*target*/) {}
        bool hasTarget() const;

        bool isAttacking = false;
        bool isTalking = false;

        StateMachine::StateMachine<Actor>* mActorStateMachine;

        ActorAnimationManager& getAnimationManager() { return mAnimation; }

        bool isPassable() { return mIsDead; }

        int32_t getId() { return mId; }

        std::string getName() const;
        void setName(const std::string& name);

        void teleport(GameLevel* level, Position pos);
        GameLevel* getLevel();

        bool attack(Actor* enemy);

        virtual bool talk(Actor* actor)
        {
            UNUSED_PARAM(actor);
            return false;
        }

        using TargetType = boost::variant<boost::blank, Actor*, ItemTarget>;
        void setTarget(TargetType target);

        MovementHandler mMoveHandler;
        TargetType mTarget;

        Position getPos() const { return mMoveHandler.getCurrentPosition(); }

        void setInvuln(bool invuln) { mInvuln = invuln; }

        // private: //TODO: fix this

        virtual void die();

        bool canTalk() const;
        bool isDead() const;
        bool isEnemy(Actor* other) const;
        std::string getActorId() const;
        void setActorId(const std::string& id);
        void setCanTalk(bool canTalk);

        bool canWalkTo(int32_t x, int32_t y);

        void attachBehaviour(Behaviour* behaviour) { mBehaviour = behaviour; };

        const std::unordered_map<std::string, std::string>& getTalkData() const { return mTalkData; }

        ActorStats mStats;

    protected:
        Behaviour* mBehaviour = nullptr;

        bool mIsDead = false;
        bool mCanTalk = false;
        Faction mFaction;

        ActorAnimationManager mAnimation;

        virtual bool canIAttack(Actor* actor);
        bool canInteractWith(Actor* actor);
        bool canTalkTo(Actor* actor);

        bool mInvuln = false;

    private:
        std::string mActorId; // TODO: this should be in an npc subclass
        std::string mName;
        int32_t mId = -1;
        // lines of talk for npcs taken from original game exe
        std::unordered_map<std::basic_string<char>, std::basic_string<char>> mTalkData;
    };
}

#endif
