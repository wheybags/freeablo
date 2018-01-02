#include "actor.h"
#include "../engine/enginemain.h"
#include "../engine/threadmanager.h"
#include "../falevelgen/random.h"
#include "../fasavegame/gameloader.h"
#include "actor/basestate.h"
#include "actorstats.h"
#include "behaviour.h"
#include "equiptarget.h"
#include "findpath.h"
#include "player.h"
#include "world.h"
#include <boost/format.hpp>
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include <diabloexe/npc.h>
#include <misc/misc.h>

namespace FAWorld
{
    const std::string Actor::typeId = "base_actor";

    void Actor::update(bool noclip)
    {
        if (!isDead())
        {
            if (getLevel())
            {
                mActorStateMachine->update(noclip);
            }

            if (mBehaviour)
                mBehaviour->update();
        }

        mAnimation.update();
    }

    Actor::Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const std::string& dieAnimPath) : mMoveHandler(World::getTicksInPeriod(1.0f))
    {
        mFaction = Faction::heaven();
        if (!dieAnimPath.empty())
            mAnimation.setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage(dieAnimPath));
        if (!walkAnimPath.empty())
            mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage(walkAnimPath));
        if (!idleAnimPath.empty())
            mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage(idleAnimPath));

        mActorStateMachine = new StateMachine::StateMachine<Actor>(new ActorState::BaseState(), this);

        mId = FAWorld::World::get()->getNewId();
    }

    Actor::Actor(const DiabloExe::Npc& npc, const DiabloExe::DiabloExe& exe) : Actor(npc.celPath, npc.celPath)
    {
        if (auto id = npc.animationSequenceId)
            mAnimation.setIdleFrameSequence(exe.getTownerAnimation()[*id]);

        mTalkData = npc.talkData;
        mNpcId = npc.id;
        mName = npc.name;
    }

    Actor::Actor(const DiabloExe::Monster& monster) : Actor("", "", "")
    {
        boost::format fmt(monster.cl2Path);
        mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage((fmt % 'w').str()));
        mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage((fmt % 'n').str()));
        mAnimation.setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage((fmt % 'd').str()));
        mAnimation.setAnimation(AnimState::attack, FARender::Renderer::get()->loadImage((fmt % 'a').str()));
        mAnimation.setAnimation(AnimState::hit, FARender::Renderer::get()->loadImage((fmt % 'h').str()));

        mBehaviour = new BasicMonsterBehaviour(this);
        mFaction = Faction::hell();
        mName = monster.monsterName;
        mSoundPath = monster.soundPath;
    }

    Actor::Actor(FASaveGame::GameLoader& loader) : mMoveHandler(loader), mAnimation(loader), mStats(loader)
    {
        mFaction = FAWorld::Faction(FAWorld::FactionType(loader.load<uint8_t>()));

        bool hasBehaviour = loader.load<bool>();
        if (hasBehaviour)
        {
            std::string typeId = loader.load<std::string>();
            mBehaviour = static_cast<Behaviour*>(World::get()->mObjectIdMapper.construct(typeId, loader));
            loader.addFunctionToRunAtEnd([this]() { mBehaviour->reAttach(this); });
        }

        mId = loader.load<int32_t>();
        mNpcId = loader.load<std::string>();
        mName = loader.load<std::string>();
        mActorStateMachine = new StateMachine::StateMachine<Actor>(new ActorState::BaseState(), this); // TODO: handle this

        // TODO: some sort of system here, so we don't need to save an npcs entire dialog
        // data into the save file every time. Probably should be done when dialog is revisited.
        uint32_t talkDataSize = loader.load<uint32_t>();
        for (uint32_t i = 0; i < talkDataSize; i++)
        {
            std::string key = loader.load<std::string>();
            mTalkData[key] = loader.load<std::string>();
        }

        mTarget.load(loader);
    }

    void Actor::save(FASaveGame::GameSaver& saver)
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

        saver.save(uint32_t(mTalkData.size()));
        for (const auto& pair : mTalkData)
        {
            saver.save(pair.first);
            saver.save(pair.second);
        }

        mTarget.save(saver);

        // TODO: handle mActorStateMachine here
    }

    Actor::~Actor()
    {
        if (mBehaviour != nullptr)
            delete mBehaviour;
    }

    void Actor::takeDamage(double amount)
    {
        if (mInvuln)
            return;

        if (amount > 10)
            amount = 60;

        mStats.takeDamage(static_cast<int32_t>(amount));
        if (!(mStats.mHp.current <= 0))
        {
            Engine::ThreadManager::get()->playSound(getHitWav());

            if (mAnimation.getCurrentAnimation() != AnimState::hit)
                mAnimation.interruptAnimation(AnimState::hit, FARender::AnimationPlayer::AnimationType::Once);
        }
    }

    void Actor::heal() { mStats.mHp = mStats.mHp.max; }

    bool Actor::hasTarget() const { return mTarget.getType() != Target::Type::None; }

    void Actor::die()
    {
        mMoveHandler.setDestination(getPos().current());
        mAnimation.playAnimation(AnimState::dead, FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
        mStats.mHp.current = 0;
        Engine::ThreadManager::get()->playSound(getDieWav());
    }

    bool Actor::isDead() const { return mStats.mHp.current <= 0; }

    bool Actor::isEnemy(Actor* other) const { return mFaction.canAttack(other->mFaction); }

    void Actor::pickupItem(Target::ItemTarget target)
    {
        auto& itemMap = getLevel()->getItemMap();
        auto tile = target.item->getTile();
        auto item = itemMap.takeItemAt(tile);
        auto dropBack = [&]() { itemMap.dropItem(std::move(item), *this, tile); };
        switch (target.action)
        {
            case Target::ItemTarget::ActionType::autoEquip:
                if (!mInventory.autoPlaceItem(*item))
                    dropBack();
                break;
            case Target::ItemTarget::ActionType::toCursor:
                auto cursorItem = mInventory.getItemAt(MakeEquipTarget<EquipTargetType::cursor>());
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

        level->addActor(this);
        mMoveHandler.teleport(level, pos);
    }

    GameLevel* Actor::getLevel() { return mMoveHandler.getLevel(); }

    std::string Actor::getDieWav() const
    {
        if (mSoundPath.empty())
            return "";

        boost::format fmt(mSoundPath);
        fmt % 'd';
        return (fmt % FALevelGen::randomInRange(1, 2)).str();
    }

    std::string Actor::getHitWav() const
    {
        if (mSoundPath.empty())
            return "";

        boost::format fmt(mSoundPath);
        fmt % 'h';
        return (fmt % FALevelGen::randomInRange(1, 2)).str();
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

    void Actor::attack(Actor* enemy)
    {
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({"sfx/misc/swing2.wav", "sfx/misc/swing.wav"}));
        enemy->takeDamage(mStats.getAttackDamage());
        if (enemy->getStats().mHp.current <= 0)
            enemy->die();
    }
}
