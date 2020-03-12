#pragma once
#include "../components/diabloexe/talkdata.h"
#include "../fagui/talkdialoguepopup.h"
#include "../farender/animationplayer.h"
#include "actor/statemachine.h"
#include "actoranimationmanager.h"
#include "actorstats.h"
#include "behaviour.h"
#include "enums.h"
#include "faction.h"
#include "gamelevel.h"
#include "inventory.h"
#include "missile/missileenums.h"
#include "movementhandler.h"
#include "position.h"
#include "spellenums.h"
#include "target.h"
#include "world.h"
#include <misc/direction.h>
#include <misc/misc.h>
#include <unordered_map>

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
        static const std::string typeId;
        virtual const std::string& getTypeId() { return typeId; }

        Actor(World& world, const std::string& walkAnimPath = "", const std::string& idleAnimPath = "", const std::string& dieAnimPath = "");
        Actor(World& world, const DiabloExe::Npc& npc, const DiabloExe::DiabloExe& exe);
        Actor(World& world, FASaveGame::GameLoader& loader);
        virtual ~Actor();
        virtual void save(FASaveGame::GameSaver& saver) const;

        virtual int32_t getOnKilledExperience() const { return 0; }
        void pickupItem(Target::ItemTarget target);
        void teleport(GameLevel* level, Position pos);
        virtual void updateSprites() {}
        GameLevel* getLevel();
        const GameLevel* getLevel() const;
        World* getWorld() const { return &mWorld; }
        virtual bool canCriticalHit() const { return false; }
        void doMeleeHit(Actor* enemy);
        void doMeleeHit(const Misc::Point& point);
        void forceAttack(Misc::Point pos);
        std::string getDieWav() const;
        std::string getHitWav() const;
        bool canIAttack(Actor* actor);
        virtual void update(bool noclip);
        void takeDamage(int32_t amount, Actor* attacker, DamageType type);
        void heal();
        void restoreMana();
        void stopMoving(std::optional<Misc::Direction> direction = std::nullopt);
        virtual void die();
        bool isDead() const;
        bool isEnemy(Actor* other) const;
        const std::unordered_map<std::string, std::string>& getMenuTalkData() const { return mMenuTalkData; }
        std::unordered_map<std::string, DiabloExe::TalkData>& getGossipData() { return mGossipData; }
        const std::unordered_map<std::string, DiabloExe::TalkData>& getGossipData() const { return mGossipData; }
        std::unordered_map<std::string, DiabloExe::QuestTalkData>& getQuestTalkData() { return mQuestTalkData; }
        DiabloExe::TalkData& getBeforeDungeonTalkData() { return mBeforeDungeonTalkData; }
        const DiabloExe::TalkData& getBeforeDungeonTalkData() const { return mBeforeDungeonTalkData; }
        const std::string& getNpcId() const { return mNpcId; }
        const std::string& getName() const { return mName; }
        const ActorStats& getStats() const { return mStats; }
        const Position& getPos() const { return mMoveHandler.getCurrentPosition(); }
        int32_t getId() const { return mId; }
        bool hasTarget() const;
        bool canTalk() const { return mMenuTalkData.size() > 0; }
        bool canInteractWith(Actor* actor);
        void dealDamageToEnemy(Actor* enemy, uint32_t damage, DamageType type);
        virtual void calculateStats(LiveActorStats& stats, const ActorStats& actorStats) const;
        const std::vector<std::unique_ptr<Missile::Missile>>& getMissiles() const { return mMissiles; }
        bool hasRangedWeaponEquipped() const;
        void doRangedAttack(Misc::Point targetPoint);
        virtual bool castSpell(SpellId spell, Misc::Point targetPoint);
        virtual void doSpellEffect(SpellId spell, Misc::Point targetPoint);
        ActorType getType() const { return mType; }
        bool isRecoveringFromHit() const;
        int32_t getMeleeHitFrame() const { return mMeleeHitFrame; }

    protected:
        void activateMissile(MissileId id, Misc::Point targetPoint);
        virtual void onEnemyKilled(Actor* enemy) { UNUSED_PARAM(enemy); };
        virtual DamageType getMeleeDamageType() const { return DamageType::Unarmed; }

    public:
        MovementHandler mMoveHandler;
        Target mTarget;
        ActorAnimationManager mAnimation;
        bool isAttacking = false;
        bool mInvuln = false;
        CharacterInventory mInventory;
        std::optional<Misc::Point> mForceAttackRequestedPoint; // this is really stupid but I don't know how else to do it
        std::optional<std::pair<SpellId, Misc::Point>> mCastSpellRequest;

        // TODO: hack, this should eventually be removed.
        // Try not to use it unless you have no other choice with the current structure.
        bool mIsTowner = false;

        ActorStats mStats;

    protected:
        std::unique_ptr<StateMachine> mActorStateMachine;
        std::string mSoundPath;
        std::unique_ptr<Behaviour> mBehaviour;
        Faction mFaction;
        std::string mName; ///< Name as it appears in-game
        int32_t mId = -1;
        std::unordered_map<std::string, std::string> mMenuTalkData;               ///< Lines of dialogue
        std::unordered_map<std::string, DiabloExe::TalkData> mGossipData;         ///< Gossip dialogues
        std::unordered_map<std::string, DiabloExe::QuestTalkData> mQuestTalkData; ///< Quest dialogues
        DiabloExe::TalkData mBeforeDungeonTalkData;
        bool mDeadLastTick = false;
        World& mWorld;
        std::vector<std::unique_ptr<Missile::Missile>> mMissiles;
        ActorType mType = ActorType::Normal;
        int32_t mMeleeHitFrame = 0; // not serialised, should be set automatically

        // TODO: this var is only used for dialog code, which branches on which npc is being spoken to.
        // Eventually, we should add a proper dialog specification system, and get rid of this.
        std::string mNpcId;
    };
}
