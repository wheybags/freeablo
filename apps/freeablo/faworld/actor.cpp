#include "actor.h"
#include "behaviour.h"

#include <misc/misc.h>

#include "actorstats.h"
#include "actor/basestate.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../engine/net/netmanager.h"
#include "../engine/enginemain.h"
#include "../falevelgen/random.h"
#include "../fasavegame/gameloader.h"
#include "player.h"
#include "findpath.h"

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(Actor)

    const std::string Actor::typeId = "base_actor";

    void Actor::setIdleAnimSequence(const std::vector<int32_t>& sequence)
    {
        mAnimation.setIdleFrameSequence (sequence);
    }

    void Actor::setTalkData(const std::unordered_map<std::basic_string<char>, std::basic_string<char>>& talkData)
    {
        mTalkData = talkData;
    }

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

    Actor::Actor(
        const std::string& walkAnimPath,
        const std::string& idleAnimPath,
        const std::string& dieAnimPath
    ) :
        mMoveHandler(World::getTicksInPeriod(1.0f)),
        mFaction(Faction::heaven())
    {
        if (!dieAnimPath.empty())
            mAnimation.setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage(dieAnimPath));
        if (!walkAnimPath.empty())
            mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage(walkAnimPath));
        if (!idleAnimPath.empty())
            mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage(idleAnimPath));

        mActorStateMachine = new StateMachine::StateMachine<Actor>(new ActorState::BaseState(), this);

        mId = FAWorld::World::get()->getNewId();
    }

    Actor::Actor(FASaveGame::GameLoader& loader)
        : mMoveHandler(loader)
        , mStats(loader)
        , mAnimation(loader)
    {
        mFaction = FAWorld::Faction(FAWorld::FactionType(loader.load<uint8_t>()));
        mIsDead = loader.load<bool>();

        bool hasBehaviour = loader.load<bool>();
        if (hasBehaviour)
        {
            std::string typeId = loader.load<std::string>();
            mBehaviour = static_cast<Behaviour*>(World::get()->mObjectIdMapper.construct(typeId, loader));
            mBehaviour->attach(this);
        }

        mId = loader.load<int32_t>();
        mActorStateMachine = new StateMachine::StateMachine<Actor>(new ActorState::BaseState(), this); // TODO: handle this
        // TODO: handle mTarget here

    }

    void Actor::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Actor", saver);

        mMoveHandler.save(saver);
        mStats.save(saver);
        mAnimation.save(saver);

        saver.save(uint8_t(mFaction.getType()));
        saver.save(mIsDead);

        bool hasBehaviour = mBehaviour != nullptr;
        saver.save(hasBehaviour);

        if (hasBehaviour)
        {
            saver.save(mBehaviour->getTypeId());
            mBehaviour->save(saver);
        }

        saver.save(mId);

        // TODO: handle mActorStateMachine here
        // TODO: handle mTarget here
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

        mStats.takeDamage(static_cast<int32_t> (amount));
        if (!(mStats.mHp.current <= 0))
        {
            Engine::ThreadManager::get()->playSound(getHitWav());

            if (mAnimation.getCurrentAnimation() != AnimState::hit)
                mAnimation.interruptAnimation(AnimState::hit, FARender::AnimationPlayer::AnimationType::Once);
        }
    }

    int32_t Actor::getCurrentHP()
    {
        return mStats.mHp.current;
    }

    bool Actor::hasTarget() const
    {
        return mTarget.type() != typeid (boost::blank);
    }

    void Actor::die()
    {
        mMoveHandler.setDestination(getPos().current());
        mAnimation.playAnimation(AnimState::dead, FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
        mIsDead = true;
        Engine::ThreadManager::get()->playSound(getDieWav());
    }

    bool Actor::isDead() const
    {
        return mIsDead;
    }

    bool Actor::isEnemy(Actor* other) const
    {
        return mFaction.canAttack(other->mFaction);
    }

    std::string Actor::getName() const
    {
        return mName;
    }

    void Actor::setName(const std::string& name)
    {
        mName = name;
    }

    void Actor::teleport(GameLevel* level, Position pos)
    {
        auto currentLevel = getLevel();
        if (currentLevel)
            currentLevel->removeActor(this);

        level->addActor(this);
        mMoveHandler.teleport(level, pos);
    }

    GameLevel* Actor::getLevel()
    {
        return mMoveHandler.getLevel();
    }

    bool Actor::canIAttack(Actor * actor)
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

    bool Actor::canInteractWith (Actor *actor) {
      if (actor == nullptr)
          return false;

      if (this == actor)
          return false;

      if (actor->isDead())
          return false;

      return true;
    }

    bool Actor::canTalkTo(Actor * actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (!actor->canTalk())
            return false;

        if (isTalking)
            return false;

        if (isEnemy(actor))
            return false;

        return true;
    }

    void Actor::setCanTalk(bool canTalk)
    {
        mCanTalk = canTalk;
    }

    bool Actor::canTalk() const
    {
        return mCanTalk;
    }

    bool Actor::canWalkTo(int32_t x, int32_t y)
    {
        return getLevel()->isPassable(x, y);
    }

    std::string Actor::getActorId() const
    {
        return mActorId;
    }

    void Actor::setActorId(const std::string& id)
    {
        mActorId = id;
    }

    bool Actor::attack(Actor *enemy)
    {
        if (enemy->isDead())
            return false;
        Engine::ThreadManager::get()->playSound(FALevelGen::chooseOne({ "sfx/misc/swing2.wav", "sfx/misc/swing.wav" }));
        enemy->takeDamage(mStats.getAttackDamage());
        if (enemy->getCurrentHP() <= 0)
            enemy->die();
        return true;
    }

    void Actor::setTarget(TargetType target)
    {
        mTarget = target;
    }
}
