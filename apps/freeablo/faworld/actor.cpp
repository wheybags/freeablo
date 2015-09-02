#include "actor.h"

#include <misc/misc.h>

#include "actorstats.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../falevelgen/random.h"
namespace FAWorld
{

    void Actor::update(bool noclip)
    {

        if(mPos.current() != mDestination)
        {
            std::pair<float, float> vector = Misc::getVec(mPos.current(), mDestination);
            Actor * enemy;
            enemy = World::get()->getActorAt(mDestination.first, mDestination.second);
            if (enemy != nullptr && mPos.distanceFrom(enemy->mPos) < 2 && this != enemy && !isAttacking && !enemy->isDead())
            {             
                    mPos.mDirection = Misc::getVecDir(vector);                    
                    mPos.update();
                    mPos.mDist = 0;

                    attack(enemy);
            }
            else if(mPos.mDist == 0 && !mAnimPlaying)
            {

                auto nextPos = Position(mPos.current().first, mPos.current().second, Misc::getVecDir(vector));
                nextPos.mMoving = true;
                World& world = *World::get();
                Actor* actorAtNext = world.getActorAt(nextPos.next().first, nextPos.next().second);
                if((noclip || ((*world.getCurrentLevel())[nextPos.next().first][nextPos.next().second].passable() &&
                                                   (actorAtNext == NULL || actorAtNext == this)))&& !mAnimPlaying)
                {
                    if(!mPos.mMoving && !mAnimPlaying)
                    {
                        mPos.mMoving = true;
                        setAnimation(AnimState::walk);                        
                    }
                }

                else if(!mAnimPlaying)
                {
                    mPos.mMoving = false;
                    mDestination = mPos.current();
                    setAnimation(AnimState::idle);

                }
                mPos.mDirection = Misc::getVecDir(vector);
            }
        }
        else if(mPos.mMoving && mPos.mDist == 0 && !mAnimPlaying)
        {
            mPos.mMoving = false;
            setAnimation(AnimState::idle);

        }

        if (!mIsDead && !mPos.mMoving && !mAnimPlaying && mAnimState != AnimState::idle)
            setAnimation(AnimState::idle);
        else if (!mIsDead && mPos.mMoving && !mAnimPlaying && mAnimState != AnimState::walk)
            setAnimation(AnimState::walk);

        if(!mAnimPlaying)
            mPos.update();
    }


    Actor::Actor(
            const std::string& walkAnimPath,
            const std::string& idleAnimPath,
            const Position& pos,
            const std::string& dieAnimPath,
            ActorStats* stats,
            const std::string& soundPath):
        mPos(pos),
        mFrame(0),
        mInventory(this),
        mSoundPath(soundPath),
        mStats(stats),
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
    }

    void Actor::takeDamage(double amount)
    {
        mStats->takeDamage(amount);
        Engine::ThreadManager::get()->playSound(getHitWav());
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
        //World::get()->deleteActorFromWorld(this);
    }

    bool Actor::isDead()
    {
        return mIsDead;
    }

    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        switch(mAnimState)
        {
            case AnimState::walk:
                return mWalkAnim;

            case AnimState::idle:
                return mIdleAnim;

            case AnimState::dead:
                return mDieAnim;

            default:
                return mIdleAnim;
        }
    }

    void Actor::setStats(ActorStats * stats)
    {
        mStats = stats;
    }

    void Actor::setAnimation(AnimState::AnimState state, bool reset)
    {
        if(mAnimState != state || reset)
        {
            mAnimState = state;
            mFrame = 0;
        }
    }

    void ActorAnimState::setClass(std::string className)
    {
        mClassName = className;
        mClassCode = className[0];
    }

    void ActorAnimState::reconstructString()
    {
        mFmt = new boost::format("plrgfx/%s/%s%s%s/%s%s%s%s.cl2");
        *mFmt % mClassName % mClassCode % mArmourCode % mWeaponCode % mClassCode % mArmourCode % mWeaponCode;
    }

    ActorAnimState::ActorAnimState(const std::string& className, const std::string& armourCode, const std::string& weaponCode, bool inDungeon)
        :mClassName(className), mArmourCode(armourCode), mWeaponCode(weaponCode), mInDungeon(inDungeon)
    {
        mClassCode = mClassName[0];
        reconstructString();
    }

    void ActorAnimState::setWeapon(std::string weaponCode)
    {        
        mWeaponCode = weaponCode;
        reconstructString();
    }

    void ActorAnimState::setArmour(std::string armourCode)
    {
        mArmourCode = armourCode;
        reconstructString();
    }

    void ActorAnimState::setDungeon(bool isDungeon)
    {
        mInDungeon = isDungeon;
    }

    std::string ActorAnimState::getAnimPath(AnimState::AnimState animState)
    {
        reconstructString();
        switch(animState)
        {
            case AnimState::dead:
                setWeapon("n");

                return (*mFmt % "dt").str();

            case AnimState::walk:

                if (mInDungeon)
                    return (*mFmt % "aw").str();
                else
                    return (*mFmt % "wl").str();
            case AnimState::meleeAttack:
                return (*mFmt % "at").str();

            default:
            case AnimState::idle:

                if (mInDungeon)
                    return (*mFmt % "as").str();
                else
                    return (*mFmt % "st").str();
        }
    }
}
