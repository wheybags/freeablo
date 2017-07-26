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
#include "player.h"
#include "findpath.h"

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(Actor)

    void Actor::update(bool noclip)
    {
        FARender::FASpriteGroup* currentAnim = nullptr;
        int32_t currentFrame = 0;
        mAnimation.getCurrentFrame(currentAnim, currentFrame);

        // play idle animation if there's nothing else going on
        if (currentAnim == nullptr)
            mAnimation.playAnimation(mIdleAnim, mAnimTimeMap[AnimState::idle], FARender::AnimationPlayer::AnimationType::Looped);

        if (mLevel)
        {
            if (isAttacking)
            {
                if (currentAnim != mAttackAnim)
                    isAttacking = false;
            }

            mActorStateMachine->update(noclip);
        }

        if (mBehaviour)
            mBehaviour->update();
    }

    size_t nextId = 0;
    size_t getNewId()
    {
        return nextId++;
    }

    Actor::Actor(
        const std::string& walkAnimPath,
        const std::string& idleAnimPath,
        const std::string& dieAnimPath
    ) :
        mFaction(Faction::heaven()),
        mMoveHandler(World::getTicksInPeriod(1.0f))
    {
        if (!dieAnimPath.empty())
        {
            mDieAnim = FARender::Renderer::get()->loadImage(dieAnimPath);
        }

        if (!walkAnimPath.empty())
            mWalkAnim = FARender::Renderer::get()->loadImage(walkAnimPath);
        if (!idleAnimPath.empty())
            mIdleAnim = FARender::Renderer::get()->loadImage(idleAnimPath);

        mAnimTimeMap[AnimState::idle] = FAWorld::World::getTicksInPeriod(0.5f);
        mAnimTimeMap[AnimState::walk] = FAWorld::World::getTicksInPeriod(0.5f);

        mActorStateMachine = new StateMachine::StateMachine<Actor>(new ActorState::BaseState(), this);

        mId = getNewId();
    }

    Actor::~Actor()
    {
        if (mStats != nullptr)
            delete mStats;
        if (mBehaviour != nullptr)
            delete mBehaviour;
    }

    void Actor::takeDamage(double amount)
    {
        mStats->takeDamage(static_cast<int32_t> (amount));
        if (!(mStats->getCurrentHP() <= 0))
        {
            Engine::ThreadManager::get()->playSound(getHitWav());
            playAnimation(AnimState::hit, FARender::AnimationPlayer::AnimationType::Once);
        }
    }

    int32_t Actor::getCurrentHP()
    {
        return mStats->getCurrentHP();
    }

    void Actor::setWalkAnimation(const std::string path)
    {
        mWalkAnim = FARender::Renderer::get()->loadImage(path);
    }

    void Actor::setIdleAnimation(const std::string path)
    {
        mIdleAnim = FARender::Renderer::get()->loadImage(path);
    }

    void Actor::die()
    {
        mMoveHandler.setDestination(getPos().current());
        playAnimation(AnimState::dead, FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
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

    void Actor::teleport(GameLevel* level, Position pos)
    {
        if (!mLevel || mLevel->getLevelIndex() != level->getLevelIndex())
        {
            if (mLevel)
                mLevel->removeActor(this);

            mLevel = level;
            mLevel->addActor(this);

            mMoveHandler.teleport(level, pos);
        }
    }

    GameLevel* Actor::getLevel()
    {
        return mLevel;
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

    bool Actor::canTalkTo(Actor * actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (getPos().distanceFrom(actor->getPos()) >= 2)
            return false;

        if (!actor->canTalk())
            return false;

        if (isTalking)
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
        return mLevel->isPassable(x, y);
    }

    std::string Actor::getActorId() const
    {
        return mActorId;
    }

    void Actor::setActorId(const std::string& id)
    {
        mActorId = id;
    }

    void Actor::getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame)
    {
        mAnimation.getCurrentFrame(sprite, frame);
    }

    void Actor::playAnimation(AnimState::AnimState state, FARender::AnimationPlayer::AnimationType type)
    {
        FARender::FASpriteGroup* sprite = nullptr;

        switch (state)
        {
            case FAWorld::AnimState::walk:
                sprite = mWalkAnim;
                break;
            case FAWorld::AnimState::idle:
                sprite = mIdleAnim;
                break;
            case FAWorld::AnimState::attack:
                sprite = mAttackAnim;
                break;
            case FAWorld::AnimState::dead:
                sprite = mDieAnim;
                break;
            case FAWorld::AnimState::hit:
                sprite = mHitAnim;
                break;
            default:
                assert(false && "BAD ENUM VALUE PASSED TO Actor::playAnimation");
        }

        Tick animLength = World::getTicksInPeriod(0.5f);
        if (mAnimTimeMap.count(state) != 0)
            animLength = mAnimTimeMap[state];

        mAnimation.playAnimation(sprite, animLength, type);
    }

    bool Actor::animationPlaying()
    {
        FARender::FASpriteGroup* sprite;
        int32_t frame;

        getCurrentFrame(sprite, frame);

        return sprite != mIdleAnim;
    }

}
