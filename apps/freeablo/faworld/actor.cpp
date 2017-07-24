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

    AnimationPlayer::AnimationPlayer(FARender::FASpriteGroup* idleAnim, Tick idleAnimDuration)
    {
        mIdleAnim = idleAnim;
        mIdleAnimDuration = idleAnimDuration;

        playAnimation(mIdleAnim, mIdleAnimDuration, AnimationType::Looped);
    }

    void AnimationPlayer::getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame)
    {
        Tick currentTick = World::get()->getCurrentTick();

        int32_t ticksIntoAnim = currentTick - mPlayingAnimStarted;
        float progress = ((float)ticksIntoAnim) / ((float)mPlayingAnimDuration);
        int32_t currentFrame = progress * mCurrentAnim->getAnimLength();

        if (currentFrame >= mCurrentAnim->getAnimLength())
        {
            if (mPlayingAnimType == AnimationType::Once)
            {
                playAnimation(mIdleAnim, mIdleAnimDuration, AnimationType::Looped);
                getCurrentFrame(sprite, currentFrame);
                return;
            }
            else if (mPlayingAnimType == AnimationType::FreezeAtEnd)
            {
                currentFrame = mCurrentAnim->getAnimLength() - 1;
            }
            else if (mPlayingAnimType == AnimationType::Looped)
            {
                currentFrame = currentFrame % mCurrentAnim->getAnimLength();
            }
        }

        frame = currentFrame;
        sprite = mCurrentAnim;
    }

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, Tick duration, AnimationPlayer::AnimationType type)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = duration;

        mPlayingAnimType = type;
        mPlayingAnimStarted = World::get()->getCurrentTick();
    }

    void AnimationPlayer::setIdleAnimation(FARender::FASpriteGroup* idleAnim, Tick idleAnimDuration)
    {
        bool restartIdle = mCurrentAnim == mIdleAnim && idleAnim != mIdleAnim;

        mIdleAnim = idleAnim;
        mIdleAnimDuration = idleAnimDuration;

        if(restartIdle)
            playAnimation(mIdleAnim, mIdleAnimDuration, AnimationType::Looped);
    }


    STATIC_HANDLE_NET_OBJECT_IN_IMPL(Actor)

    void Actor::update(bool noclip)
    {
        if (mLevel)
        {
            if (isAttacking)
            {
                FARender::FASpriteGroup* currentAnim = nullptr;
                int32_t currentFrame = 0;
                mAnimation.getCurrentFrame(currentAnim, currentFrame);

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
        const Position& pos,
        const std::string& dieAnimPath
    ) :
        mPos(pos),
        mFaction(Faction::heaven())
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
        mAnimTimeMap[AnimState::idle] = FAWorld::World::getTicksInPeriod(0.5f);
        mAnimTimeMap[AnimState::walk] = FAWorld::World::getTicksInPeriod(0.5f);

        mAnimation = AnimationPlayer(mIdleAnim, mAnimTimeMap[AnimState::idle]);

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
            playAnimation(AnimState::hit, AnimationPlayer::AnimationType::Once);
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
        mAnimation.setIdleAnimation(mIdleAnim, mAnimTimeMap[AnimState::idle]);
    }

    void Actor::die()
    {
        mDestination = mPos.mGoal = mPos.current();
        mPos.mMoving = false;
        playAnimation(AnimState::dead, AnimationPlayer::AnimationType::FreezeAtEnd);
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

    bool Actor::findPath(GameLevelImpl * level, std::pair<int32_t, int32_t> destination)
    {
        bool bArrivable = false;
        mPos.mPath = std::move(FindPath::get(level)->find(mPos.current(), destination, bArrivable));
        //mPos.mGoal = destination; // destination maybe changed by findPath.
        mPos.mIndex = 0;
        return bArrivable;
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

    void Actor::getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame)
    {
        mAnimation.getCurrentFrame(sprite, frame);
    }

    void Actor::playAnimation(AnimState::AnimState state, AnimationPlayer::AnimationType type)
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
