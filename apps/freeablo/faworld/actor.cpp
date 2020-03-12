#include "actor.h"
#include "../engine/enginemain.h"
#include "../engine/threadmanager.h"
#include "../fasavegame/gameloader.h"
#include "actor/attackstate.h"
#include "actor/basestate.h"
#include "actorstats.h"
#include "behaviour.h"
#include "equiptarget.h"
#include "findpath.h"
#include "missile/missile.h"
#include "player.h"
#include "spells.h"
#include "world.h"
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include <diabloexe/npc.h>
#include <fmt/format.h>
#include <misc/misc.h>
#include <random/random.h>

namespace FAWorld
{
    const std::string Actor::typeId = "base_actor";

    void Actor::update(bool noclip)
    {
        if (!isDead())
        {
            if (getLevel() && !isRecoveringFromHit())
                mActorStateMachine->update(noclip);

            if (mBehaviour)
                mBehaviour->update();
        }
        else if (!mDeadLastTick)
        {
            if (getLevel())
            {
                getLevel()->actorMapRemove(this, getPos().current());
                getLevel()->actorMapRemove(this, getPos().next());
            }

            mDeadLastTick = true;
        }

        mAnimation.update();

        for (auto& missile : mMissiles)
            missile->update();
        mMissiles.erase(
            std::remove_if(mMissiles.begin(), mMissiles.end(), [](const std::unique_ptr<Missile::Missile>& missile) { return missile->isComplete(); }),
            mMissiles.end());
    }

    Actor::Actor(World& world, const std::string& walkAnimPath, const std::string& idleAnimPath, const std::string& dieAnimPath) : mStats(*this), mWorld(world)
    {
        mStats.initialise(BaseStats());

        mFaction = Faction::heaven();
        if (!dieAnimPath.empty())
            mAnimation.setAnimationSprites(AnimState::dead, FARender::Renderer::get()->loadImage(dieAnimPath));
        if (!walkAnimPath.empty())
            mAnimation.setAnimationSprites(AnimState::walk, FARender::Renderer::get()->loadImage(walkAnimPath));
        if (!idleAnimPath.empty())
            mAnimation.setAnimationSprites(AnimState::idle, FARender::Renderer::get()->loadImage(idleAnimPath));

        mActorStateMachine.reset(new StateMachine(this, new ActorState::BaseState()));

        mId = mWorld.getNewId();
    }

    Actor::Actor(World& world, const DiabloExe::Npc& npc, const DiabloExe::DiabloExe& exe) : Actor(world, npc.celPath, npc.celPath)
    {
        if (auto id = npc.animationSequenceId)
            mAnimation.setIdleFrameSequence(exe.getTownerAnimation()[*id]);

        mMenuTalkData = npc.menuTalkData;
        mGossipData = npc.gossipData;
        mQuestTalkData = npc.questTalkData;
        mBeforeDungeonTalkData = npc.beforeDungeonTalkData;
        mNpcId = npc.id;
        mName = npc.name;

        mIsTowner = true;
    }

    Actor::Actor(World& world, FASaveGame::GameLoader& loader) : mMoveHandler(loader), mAnimation(loader), mStats(*this, loader), mWorld(world)
    {
        mFaction = FAWorld::Faction(FAWorld::FactionType(loader.load<uint8_t>()));

        bool hasBehaviour = loader.load<bool>();
        if (hasBehaviour)
        {
            std::string typeId = loader.load<std::string>();
            mBehaviour.reset(static_cast<Behaviour*>(mWorld.mObjectIdMapper.construct(typeId, loader)));
            loader.addFunctionToRunAtEnd([this]() { mBehaviour->reAttach(this); });
        }

        mId = loader.load<int32_t>();
        mNpcId = loader.load<std::string>();
        mName = loader.load<std::string>();
        mIsTowner = loader.load<bool>();
        mDeadLastTick = loader.load<bool>();

        // TODO: some sort of system here, so we don't need to save an npcs entire dialog
        // data into the save file every time. Probably should be done when dialog is revisited.
        uint32_t talkDataSize = loader.load<uint32_t>();
        for (uint32_t i = 0; i < talkDataSize; i++)
        {
            std::string key = loader.load<std::string>();
            mMenuTalkData[key] = loader.load<std::string>();
        }

        mTarget.load(loader);
        mInventory.load(loader);

        mActorStateMachine.reset(new StateMachine(this));
        mActorStateMachine->load(loader);

        uint32_t missilesSize = loader.load<uint32_t>();
        mMissiles.reserve(missilesSize);
        for (uint32_t i = 0; i < missilesSize; i++)
            mMissiles.push_back(std::make_unique<Missile::Missile>(loader));

        mType = ActorType(loader.load<uint8_t>());
    }

    void Actor::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("Actor", saver);

        mMoveHandler.save(saver);
        mAnimation.save(saver);
        mStats.save(saver);

        saver.save(uint8_t(mFaction.getType()));

        bool hasBehaviour = mBehaviour != nullptr;
        saver.save(hasBehaviour);

        if (hasBehaviour)
        {
            saver.save(mBehaviour->getTypeId());
            mBehaviour->save(saver);
        }

        saver.save(mId);
        saver.save(mNpcId);
        saver.save(mName);
        saver.save(mIsTowner);
        saver.save(mDeadLastTick);

        saver.save(uint32_t(mMenuTalkData.size()));
        for (const auto& pair : mMenuTalkData)
        {
            saver.save(pair.first);
            saver.save(pair.second);
        }

        mTarget.save(saver);
        mInventory.save(saver);

        mActorStateMachine->save(saver);

        saver.save(static_cast<uint32_t>(mMissiles.size()));
        for (auto& missile : mMissiles)
            missile->save(saver);

        saver.save(uint8_t(mType));
    }

    Actor::~Actor() = default;

    void Actor::takeDamage(int32_t amount, Actor* attacker, DamageType type)
    {
        if (mInvuln)
            return;

        // https://wheybags.gitlab.io/jarulfs-guide/#getting-hit
        int32_t blockChance = getStats().getCalculatedStats().blockChance;
        blockChance += 2 * (getStats().mLevel - attacker->getStats().mLevel);

        if (!mMoveHandler.moving() && mAnimation.getCurrentAnimation() != AnimState::hit && mWorld.mRng->randomInRange(0, 99) < blockChance)
        {
            mAnimation.interruptAnimation(AnimState::block, FARender::AnimationPlayer::AnimationType::Once);
#ifdef DEBUG_MELEE_COMBAT
            puts("blocked");
#endif
            return;
        }

#ifdef DEBUG_MELEE_COMBAT
        puts("hit");
#endif

        // https://wheybags.gitlab.io/jarulfs-guide/#how-to-calculate-monster-data
        if (mType == ActorType::Undead && type == DamageType::Club)
            amount += amount / 2;
        if (mType == ActorType::Undead && type == DamageType::Sword)
            amount -= amount / 2;
        if (mType == ActorType::Animal && type == DamageType::Club)
            amount -= amount / 2;
        if (mType == ActorType::Animal && type == DamageType::Sword)
            amount += amount / 2;

        mStats.takeDamage(static_cast<int32_t>(amount));

        if (mStats.getHp().current > 0)
        {
            Engine::ThreadManager::get()->playSound(getHitWav()); // TODO: should this only play when doing hit recovery?

            if (amount >= mStats.getCalculatedStats().hitRecoveryDamageThreshold)
                mAnimation.interruptAnimation(AnimState::hit, FARender::AnimationPlayer::AnimationType::Once);
        }

        if (getStats().getHp().current <= 0)
            die();
    }

    void Actor::heal() { mStats.getHp().current = mStats.getHp().max; }

    void Actor::restoreMana() { mStats.getMana().current = mStats.getMana().max; }

    void Actor::stopMoving(std::optional<Misc::Direction> direction) { mMoveHandler.stopMoving(*this, direction); }

    bool Actor::hasTarget() const { return mTarget.getType() != Target::Type::None; }

    void Actor::die()
    {
        mMoveHandler.setDestination(getPos().current());
        mAnimation.playAnimation(AnimState::dead, FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
        mStats.getHp().current = 0;
        Engine::ThreadManager::get()->playSound(getDieWav());
    }

    bool Actor::isDead() const { return mStats.getHp().current <= 0; }

    bool Actor::isEnemy(Actor* other) const { return mFaction.canAttack(other->mFaction); }

    void Actor::pickupItem(Target::ItemTarget target)
    {
        ItemMap& itemMap = getLevel()->getItemMap();
        std::unique_ptr<Item> item = itemMap.takeItemAt(target.itemLocation);

        if (!item)
            return;

        auto dropBack = [&]() { itemMap.dropItem(std::move(item), *this, target.itemLocation); };
        switch (target.action)
        {
            case Target::ItemTarget::ActionType::autoEquip:
                if (!mInventory.autoPlaceItem(*item))
                    dropBack();
                break;
            case Target::ItemTarget::ActionType::toCursor:
                auto cursorItem = mInventory.getCursorHeld();
                if (!cursorItem.isEmpty())
                    return dropBack();

                mInventory.setCursorHeld(*item);
                break;
        }
    }

    void Actor::teleport(GameLevel* level, Position pos)
    {
        auto currentLevel = getLevel();
        if (currentLevel)
            currentLevel->removeActor(this);

        mMoveHandler.teleport(level, pos);
        level->insertActor(this);

        updateSprites();
    }

    GameLevel* Actor::getLevel() { return mMoveHandler.getLevel(); }
    const GameLevel* Actor::getLevel() const { return mMoveHandler.getLevel(); }

    std::string Actor::getDieWav() const
    {
        if (mSoundPath.empty())
            return "";

        return fmt::format(mSoundPath, 'd', mWorld.mRng->randomInRange(1, 2));
    }

    std::string Actor::getHitWav() const
    {
        if (mSoundPath.empty())
            return "";

        return fmt::format(mSoundPath, 'h', mWorld.mRng->randomInRange(1, 2));
    }

    bool Actor::canIAttack(Actor* actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (!isEnemy(actor))
            return false;

        if (actor->isDead())
            return false;
        return true;
    }

    bool Actor::canInteractWith(Actor* actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (actor->isDead())
            return false;

        return true;
    }

    void Actor::dealDamageToEnemy(Actor* enemy, uint32_t damage, DamageType type)
    {
        enemy->takeDamage(damage, this, type);
        if (enemy->isDead())
            onEnemyKilled(enemy);
    }

    void Actor::calculateStats(LiveActorStats& stats, const ActorStats&) const
    {
        stats = LiveActorStats();
        stats.maxLife = 10;
    }

    bool Actor::isRecoveringFromHit() const
    {
        return mAnimation.getCurrentAnimation() == AnimState::hit || mAnimation.getCurrentAnimation() == AnimState::block;
    }

    void Actor::doMeleeHit(const Misc::Point& point)
    {
        auto actor = getLevel()->getActorAt(point);
        if (!canIAttack(actor))
            return;
        doMeleeHit(actor);
    }

    void Actor::forceAttack(Misc::Point pos) { mForceAttackRequestedPoint = pos; }

    void Actor::doMeleeHit(Actor* enemy)
    {
        Engine::ThreadManager::get()->playSound(mWorld.mRng->chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));

        const LiveActorStats& stats = mStats.getCalculatedStats();
        int32_t toHit = stats.toHitMelee.getCombined();
        toHit -= enemy->getStats().getCalculatedStats().armorClass;
        toHit = Misc::clamp(toHit, stats.toHitMeleeMinMaxCap.min, stats.toHitMeleeMinMaxCap.max);
        int32_t roll = mWorld.mRng->randomInRange(0, 99); // TODO: should this be (1,100) instead of (0, 99)?

#ifdef DEBUG_MELEE_COMBAT
        printf("%s melee attacks %s - ", mName.c_str(), enemy->mName.c_str());
#endif

        if (roll < toHit)
        {
            int32_t damage = stats.meleeDamage;
            damage += mWorld.mRng->randomInRange(stats.meleeDamageBonusRange.start, stats.meleeDamageBonusRange.end);
            if (canCriticalHit() && mWorld.mRng->randomInRange(0, 99) < mStats.mLevel)
            {
                damage *= 2;
#ifdef DEBUG_MELEE_COMBAT
                printf("critical hit for %d - ", damage);
#endif
            }
            else
            {
#ifdef DEBUG_MELEE_COMBAT
                printf("hit for %d - ", damage);
#endif
            }

            dealDamageToEnemy(enemy, damage, getMeleeDamageType());
        }
        else
        {
#ifdef DEBUG_MELEE_COMBAT
            puts("miss");
#endif
        }
    }

    bool Actor::hasRangedWeaponEquipped() const { return mInventory.isRangedWeaponEquipped(); }

    void Actor::doRangedAttack(Misc::Point targetPoint)
    {
        Engine::ThreadManager::get()->playSound("sfx/misc/bfire.wav");
        // Note: Fire and lightning arrows are also possible.
        activateMissile(MissileId::arrow, targetPoint);
    }

    bool Actor::castSpell(SpellId spell, Misc::Point targetPoint)
    {
        mCastSpellRequest = std::pair(spell, targetPoint);
        return true;
    }

    void Actor::doSpellEffect(SpellId spell, Misc::Point targetPoint)
    {
        auto spellData = SpellData(spell);
        Engine::ThreadManager::get()->playSound(spellData.soundEffect());
        for (auto missileId : spellData.missiles())
            activateMissile(missileId, targetPoint);
    }

    void Actor::activateMissile(MissileId id, Misc::Point targetPoint)
    {
        auto missile = std::make_unique<Missile::Missile>(id, *this, targetPoint);
        mMissiles.push_back(std::move(missile));
    }
}
