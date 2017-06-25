#include "actor.h"

#include <misc/misc.h>

#include "actorstats.h"
#include "actor/basestate.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../engine/net/netmanager.h"
#include "../falevelgen/random.h"
#include "player.h"
#include "findpath.h"

namespace FAWorld
{
    STATIC_HANDLE_NET_OBJECT_IN_IMPL(Actor)

    void Actor::update(bool noclip, size_t ticksPassed)
    {
        if (mLevel)
        {
            size_t animDivisor = mAnimTimeMap[getAnimState()];
            if (animDivisor == 0)
            {
                animDivisor = FAWorld::World::getTicksInPeriod(0.1f);
            }
            bool advanceAnims = !(ticksPassed % (World::ticksPerSecond / animDivisor));

            if (advanceAnims)
            {
                auto currentAnim = getCurrentAnim();

                if (mAnimPlaying)
                {
                    if (mFrame < currentAnim->getAnimLength())
                        mFrame++;

                    if (mFrame >= currentAnim->getAnimLength())
                    {
                        if (currentAnim == mAttackAnim)
                            isAttacking = false;

                        mAnimPlaying = false;
                        mFrame--;
                    }
                }
                else {
                    if (!isDead())
                        mFrame = (mFrame + 1) % currentAnim->getAnimLength();
                    else if (mFrame < currentAnim->getAnimLength() - 1)
                        mFrame++;
                }

#ifndef NDEBUG
                assert(mFrame < getCurrentAnim()->getAnimLength());
#endif
            }

            if (!mActorState.empty()) {
                if (auto next = mActorState.back()->update(*this, noclip, ticksPassed)) {
                    mActorState.back()->onExit(*this);

                    switch (next->op) {
                        case StateOperation::pop:
                            mActorState.pop_back();
                            break;
                        case StateOperation::push:
                            mActorState.push_back(next->nextState);
                        case StateOperation::replace:
                            mActorState.pop_back();
                            mActorState.push_back(next->nextState);
                            break;
                    }

                    mActorState.back()->onEnter(*this);
                }
            }
        }
    }

    size_t nextId = 0;
    size_t getNewId()
    {
        return nextId++;
    }

    Actor::Actor(
        const std::string& walkAnimPath,
        const std::string& idleAnimPath,
        const Position& pos,
        const std::string& dieAnimPath
    ) :
        mPos(pos),
        mFrame(0),
        mFaction(Faction::heaven()),
        mAnimState(AnimState::idle)
    {
        if (!dieAnimPath.empty())
        {
            mDieAnim = FARender::Renderer::get()->loadImage(dieAnimPath);
        }

        if (!walkAnimPath.empty())
            mWalkAnim = FARender::Renderer::get()->loadImage(walkAnimPath);
        if (!idleAnimPath.empty())
            mIdleAnim = FARender::Renderer::get()->loadImage(idleAnimPath);
        mDestination = mPos.current();
        mAnimTimeMap[AnimState::idle] = FAWorld::World::getTicksInPeriod(0.1f);
        mAnimTimeMap[AnimState::walk] = FAWorld::World::getTicksInPeriod(0.1f);

        mActorState.push_back(new ActorState::BaseState());

        mId = getNewId();
    }

    Actor::~Actor()
    {
        if (mStats != nullptr)
            delete mStats;
    }

    void Actor::takeDamage(double amount)
    {
        mStats->takeDamage(static_cast<int32_t> (amount));
        if (!(mStats->getCurrentHP() <= 0))
        {
            Engine::ThreadManager::get()->playSound(getHitWav());
            setAnimation(AnimState::hit);
            mAnimPlaying = true;
        }
        else
            mAnimPlaying = false;
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
        setAnimation(AnimState::dead);
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

    AnimState::AnimState Actor::getAnimState()
    {
        return mAnimState;
    }

    bool Actor::findPath(GameLevelImpl * level, std::pair<int32_t, int32_t> destination)
    {
        bool bArrivable = false;
        mPos.mPath = std::move(FindPath::get(level)->find(mPos.current(), destination, bArrivable));
        mPos.mGoal = destination; // destination maybe changed by findPath.
        mPos.mIndex = 0;
        return bArrivable;
    }

    FARender::FASpriteGroup* Actor::getCurrentAnim()
    {
        FARender::FASpriteGroup* retval;

        switch (mAnimState)
        {
        case AnimState::walk:
            retval = mWalkAnim;
            break;

        case AnimState::idle:
            retval = mIdleAnim;
            break;

        case AnimState::attack:
            retval = mAttackAnim;
            break;

        case AnimState::dead:
            retval = mDieAnim;
            break;

        case AnimState::hit:
            retval = mHitAnim;
            break;

        default:
            retval = mIdleAnim;
            break;
        }

        if (!retval || !retval->isValid())
            retval = mIdleAnim;

        return retval;
    }

    void Actor::setAnimation(AnimState::AnimState state, bool reset)
    {
        if (mAnimState != state || reset)
        {
            mAnimState = state;
            mFrame = 0;
        }
    }

    void Actor::setLevel(GameLevel* level)
    {
        if (!mLevel || mLevel->getLevelIndex() != level->getLevelIndex())
        {
            if (mLevel)
                mLevel->removeActor(this);

            mLevel = level;
            mLevel->addActor(this);
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

        if (mPos.distanceFrom(actor->mPos) >= 2)
            return false;

        if (isAttacking)
            return false;

        return true;
    }

    bool Actor::canTalkTo(Actor * actor)
    {
        if (actor == nullptr)
            return false;

        if (this == actor)
            return false;

        if (mPos.distanceFrom(actor->mPos) >= 2)
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

    std::string Actor::getActorId() const
    {
        return mActorId;
    }

    void Actor::setActorId(const std::string& id)
    {
        mActorId = id;
    }
}
