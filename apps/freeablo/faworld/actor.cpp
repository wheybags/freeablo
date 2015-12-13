#include "actor.h"

#include <misc/misc.h>

#include "actorstats.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../engine/netmanager.h"
#include "../falevelgen/random.h"

namespace FAWorld
{

    void Actor::update(bool noclip, size_t ticksPassed)
    {
        if(mLevel)
        {
            size_t animDivisor = mAnimTimeMap[getAnimState()];
            if(animDivisor == 0)
            {
                animDivisor=12;
            }
            bool advanceAnims  = !(ticksPassed % (World::ticksPerSecond/animDivisor));

            if(advanceAnims)
            {

                if(!mAnimPlaying && !isDead())
                {
                    mFrame = fmod((mFrame + 1), (double)(getCurrentAnim().animLength));
                }

                else if(mAnimPlaying && mFrame <= getCurrentAnim().animLength-1)
                {
                    mFrame++;
                }

                else if(mAnimPlaying && mFrame == getCurrentAnim().animLength)
                {
                    mAnimPlaying = false;
                }

                else if(!mAnimPlaying && mFrame < getCurrentAnim().animLength-1)
                {
                    mFrame++;
                }
            }

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

                    FAWorld::Actor* actorAtNext = world.getActorAt(nextPos.next().first, nextPos.next().second);

                    if((noclip || (mLevel->getTile(nextPos.next().first, nextPos.next().second).passable() &&
                                                       (actorAtNext == NULL || actorAtNext == this))) && !mAnimPlaying)
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
    }


    Actor::Actor(
            const std::string& walkAnimPath,
            const std::string& idleAnimPath,
            const Position& pos,
            const std::string& dieAnimPath,
            ActorStats* stats):
        mPos(pos),
        mFrame(0),        
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
        mAnimTimeMap[AnimState::idle] = 10;
        mAnimTimeMap[AnimState::walk] = 10;


    }

    Actor::~Actor()
    {
        if(mStats != nullptr)
            delete mStats;
    }

    void Actor::takeDamage(double amount)
    {
        mStats->takeDamage(amount);
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

    bool Actor::isDead()
    {
        return mIsDead;
    }

    AnimState::AnimState Actor::getAnimState()
    {
        return mAnimState;
    }

    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        FARender::FASpriteGroup retval;
        
        switch(mAnimState)
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
        
        if(!retval.isValid())
            retval = mIdleAnim;
        
        return retval;
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

    struct ActorNetData
    {
        size_t frame;
        uint8_t animState;
        size_t destX;
        size_t destY;
        int32_t levelIndex;
    };

    size_t Actor::getWriteSize()
    {
        return mPos.getWriteSize() + sizeof(ActorNetData);
    }

    bool Actor::writeTo(ENetPacket *packet, size_t& position)
    {
        mPos.startWriting();
        if(!mPos.writeTo(packet, position))
            return false;

        ActorNetData data;
        data.frame = mFrame;
        data.animState = mAnimState;
        data.destX = mDestination.first;
        data.destY = mDestination.second;

        if(mLevel)
            data.levelIndex = mLevel->getLevelIndex();
        else
            data.levelIndex = -1;

        return Engine::writeToPacket(packet, position, data);
    }

    bool Actor::readFrom(ENetPacket *packet, size_t& position)
    {
        if(!mPos.readFrom(packet, position))
            return false;

        ActorNetData data;
        if(Engine::readFromPacket(packet, position, data))
        {
            mFrame = data.frame;
            mAnimState = (AnimState::AnimState)data.animState;

            if(World::get()->getCurrentPlayer() != this)
            {
                // don't want to read destination for our player object,
                // we keep track of our own destination
                mDestination.first = data.destX;
                mDestination.second = data.destY;

                setLevel(World::get()->getLevel(data.levelIndex));
            }

            return true;
        }

        return false;
    }

    void Actor::setLevel(GameLevel* level)
    {
        if(!mLevel || mLevel->getLevelIndex() != level->getLevelIndex())
        {
            if(mLevel)
                mLevel->removeActor(this);

            mLevel = level;
            mLevel->addActor(this);
        }
    }

    GameLevel* Actor::getLevel()
    {
        return mLevel;
    }
}
