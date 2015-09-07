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


    Actor::Actor(const std::string& walkAnimPath,
            const std::string& idleAnimPath,
            const Position& pos,
            const std::string& dieAnimPath):
        mPos(pos),
        mFrame(0),        
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

    AnimState::AnimState Actor::getAnimState()
    {
        return mAnimState;
    }

    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        switch(mAnimState)
        {
            case AnimState::walk:
                return mWalkAnim;

            case AnimState::idle:
                return mIdleAnim;
            default:
                return mIdleAnim;
        }
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
    };

    size_t Actor::getSize()
    {
        return mPos.getSize() + sizeof(ActorNetData);
    }

    size_t Actor::writeTo(ENetPacket *packet, size_t start)
    {
        start = mPos.writeTo(packet, start);

        ActorNetData* data = (ActorNetData*)(packet->data + start);
        data->frame = mFrame;
        data->animState = mAnimState;
        data->destX = mDestination.first;
        data->destY = mDestination.second;

        return start + sizeof(ActorNetData);
    }

    size_t Actor::readFrom(ENetPacket *packet, size_t start)
    {
        start = mPos.readFrom(packet, start);

        ActorNetData* data = (ActorNetData*)(packet->data + start);
        mFrame = data->frame;
        mAnimState = (AnimState::AnimState)data->animState;

        // don't want to read destination for our player object,
        // we keep track of our own destination
        if(World::get()->getCurrentPlayer() != this)
        {
            mDestination.first = data->destX;
            mDestination.second = data->destY;
        }

        return start + sizeof(ActorNetData);
    }
}
