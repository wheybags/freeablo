#include "actor.h"

#include <misc/misc.h>

#include "world.h"

namespace FAWorld
{
    void Actor::update(bool noclip)
    {
        if(mPos.current() != mDestination)
        {
            if(mPos.mDist == 0)
            {
                std::pair<float, float> vector = Misc::getVec(mPos.current(), mDestination);
                auto nextPos = Position(mPos.current().first, mPos.current().second, Misc::getVecDir(vector));
                nextPos.mMoving = true;
                World& world = *World::get();
                FAWorld::Actor* actorAtNext = world.getActorAt(nextPos.next().first, nextPos.next().second);

                if(noclip || ((*world.getCurrentLevel())[nextPos.next().first][nextPos.next().second].passable() &&
                                                   (actorAtNext == NULL || actorAtNext == this)))
                {
                    if(!mPos.mMoving)
                    {
                        mPos.mMoving = true;
                        setAnimation(FAWorld::AnimState::walk);
                    }
                }
                else
                {
                    mPos.mMoving = false;
                    mDestination = mPos.current();
                    setAnimation(FAWorld::AnimState::idle);
                }

                mPos.mDirection = Misc::getVecDir(vector);
            }
        }
        else if(mPos.mMoving && mPos.mDist == 0)
        {
            mPos.mMoving = false;
            setAnimation(FAWorld::AnimState::idle);
        }

        mPos.update(); 
    }
    Actor::Actor(const std::string& walkAnimPath, const std::string& idleAnimPath, const Position& pos):
        mPos(pos),
        mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)),
        mIdleAnim(FARender::Renderer::get()->loadImage(idleAnimPath)),
        mFrame(0),
        mInventory(this),        
        mAnimState(AnimState::idle)
    {
        mDestination = mPos.current();
    }

    void Actor::setWalkAnimation(const std::string path)
    {
        mWalkAnim = FARender::Renderer::get()->loadImage(path);
    }

    void Actor::setIdleAnimation(const std::string path)
    {
        mIdleAnim = FARender::Renderer::get()->loadImage(path);
    }

    FARender::FASpriteGroup Actor::getCurrentAnim()
    {
        switch(mAnimState)
        {
            case AnimState::walk:
                return mWalkAnim;

            default: // AnimState::idle:
                return mIdleAnim;
        }
    }
    void Actor::setStats(ActorStats * stats)
    {
        mStats = stats;

    }

    void Actor::setAnimation(AnimState::AnimState state)
    {
        if(mAnimState != state)
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
