#include "actor.h"

#include <misc/misc.h>

#include "actorstats.h"
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

            if (mPos.mGoal != std::pair<int32_t, int32_t>(0, 0) && mPos.current() != mPos.mGoal)
            {
                Actor * actor;
                actor = World::get()->getActorAt(mDestination.first, mDestination.second);
                if (canIAttack(actor))
                {
                    std::pair<float, float> vector = Misc::getVec(mPos.current(), mDestination);
                    mPos.setDirection(Misc::getVecDir(vector));
                    mPos.update();
                    mPos.mDist = 0;

                    attack(actor);
                }
                else if (canTalkTo(actor))
                {
                    mPos.mDist = 0;
                    talk(actor);
                }
                else if (mPos.mDist == 0)
                {
                    World& world = *World::get();
                    if (!mPos.mPath.size() || mPos.mIndex == -1 || mPos.mPath.back() != mDestination)
                    {
                        findPath(world.getCurrentLevel(), mPos.mGoal);
                    }
                    if (!mAnimPlaying)
                    {
                        if (mPos.current() == mPos.mGoal)        //the mPos.mGoal maybe changed by the findPath call.so we need judge it again.
                        {
                            mPos.mMoving = false;
                            setAnimation(AnimState::idle);
                        }
                        else
                        {
                            auto nextPos = mPos.pathNext(false);
                            FAWorld::Actor* actorAtNext = world.getActorAt(nextPos.first, nextPos.second);

                            if ((noclip || (mLevel->isPassable(nextPos.first, nextPos.second) &&
                                (actorAtNext == NULL || actorAtNext == this || actorAtNext->isDead()))) && !mAnimPlaying)
                            {
                                if (!mPos.mMoving && !mAnimPlaying)
                                {
                                    mPos.mMoving = true;
                                    setAnimation(AnimState::walk);
                                }
                            }
                            else if (!mAnimPlaying)
                            {
                                mPos.mMoving = false;
                                mDestination = mPos.current();
                                setAnimation(AnimState::idle);
                            }

                            mPos.setDirection(Misc::getVecDir(Misc::getVec(mPos.current(), nextPos)));
                        }
                    }
                }
            }
            else if (mPos.mMoving && mPos.mDist == 0 && !mAnimPlaying && !isDead())
            {
                mPos.mMoving = false;
                setAnimation(AnimState::idle);
            }

            if (!mIsDead && !mPos.mMoving && !mAnimPlaying && mAnimState != AnimState::idle)
                setAnimation(AnimState::idle);
            else if (!mIsDead && mPos.mMoving && !mAnimPlaying && mAnimState != AnimState::walk)
                setAnimation(AnimState::walk);

            //if walk anim finished,we need move the role to next tiled
            if (!mAnimPlaying) {
                mPos.update();
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
        mDestination = mPos.mGoal = mPos.current();
        mPos.mMoving = false;
        setAnimation(AnimState::dead, true);
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
}
