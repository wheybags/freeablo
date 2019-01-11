#pragma once
#include "../farender/animationplayer.h"
#include "actor/statemachine.h"
#include "actoranimationmanager.h"
#include "actorstats.h"
#include "behaviour.h"
#include "faction.h"
#include "gamelevel.h"
#include "inventory.h"
#include "movementhandler.h"
#include "position.h"
#include "target.h"
#include "world.h"
#include <boost/format.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <map>
#include <misc/direction.h>
#include <misc/misc.h>

namespace Random
{
    class Rng;
}

namespace FASaveGame
{
    class Loader;
    class Saver;
}

namespace FAWorld
{
    class Behaviour;
    class World;

    class Actor
    {
    public:
        Actor(World& world, const std::string& walkAnimPath = "", const std::string& idleAnimPath = "", const std::string& dieAnimPath = "");
        Actor(World& world, const DiabloExe::Npc& npc, const DiabloExe::DiabloExe& exe);
        Actor(World& world, Random::Rng& rng, const DiabloExe::Monster& monster);
        Actor(World& world, FASaveGame::GameLoader& loader);
        virtual ~Actor();
        virtual int getArmor() const { /*placeholder */ return 0; }

        virtual void save(FASaveGame::GameSaver& saver);
        virtual bool checkHit(Actor* enemy);

        static const std::string typeId;
        virtual const std::string& getTypeId() { return typeId; }

        void pickupItem(Target::ItemTarget target);

        void teleport(GameLevel* level, Position pos);
        virtual void updateSprites() {}
        GameLevel* getLevel();
        World* getWorld() const { return &mWorld; }

        virtual int32_t meleeDamageVs(const Actor* actor) const;
        void doMeleeHit(Actor* enemy);
        void doMeleeHit(const Misc::Point& point);
        void startMeleeAttack(Misc::Direction direction);
        void checkDeath();

        std::string getDieWav() const;
        std::string getHitWav() const;

        bool canIAttack(Actor* actor);
        virtual void update(bool noclip);
        void init();
        void takeDamage(int32_t amount);
        void heal();
        void stopAndPointInDirection(Misc::Direction direction);

        void die();
        bool isDead() const;
        bool isEnemy(Actor* other) const;

        const std::map<std::string, std::string>& getTalkData() const { return mTalkData; }
        const std::string& getNpcId() const { return mNpcId; }
        const std::string& getName() const { return mName; }
        const ActorStats& getStats() const { return mStats; }
        const Position& getPos() const { return mMoveHandler.getCurrentPosition(); }
        int32_t getId() const { return mId; }
        bool isPassable() const { return isDead(); }
        bool hasTarget() const;

        bool canTalk() const { return mTalkData.size() > 0; }
        bool canInteractWith(Actor* actor);

        // public member variables
        MovementHandler mMoveHandler;
        Target mTarget;
        ActorAnimationManager mAnimation;
        bool isAttacking = false;
        bool mInvuln = false;
        CharacterInventory mInventory;
        boost::optional<Misc::Direction> mMeleeAttackRequestedDirection; // this is really stupid but I don't know how else to do it

        // TODO: hack, this should eventually be removed.
        // Try not to use it unless you have no other choice with the current structure.
        bool mIsTowner = false;

    protected:
        // protected member variables
        std::unique_ptr<StateMachine> mActorStateMachine;
        ActorStats mStats;
        std::string mSoundPath;
        std::unique_ptr<Behaviour> mBehaviour;
        Faction mFaction;
        std::string mName; ///< Name as it appears in-game
        int32_t mId = -1;
        std::map<std::string, std::string> mTalkData; ///< Lines of dialogue
        World& mWorld;

        // TODO: this var is only used for dialog code, which branches on which npc is being spoken to.
        // Eventually, we should add a proper dialog specification system, and get rid of this.
        std::string mNpcId;
    };
}
