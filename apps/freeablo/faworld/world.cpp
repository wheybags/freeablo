#include "world.h"

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"

namespace FAWorld
{
    World::World()
    {
        mActors.push_back(&mPlayer);
        mTicksSinceLastAnimUpdate = 0;
    }

    void World::update()
    {
        mTicksSinceLastAnimUpdate++;

        bool advanceAnims = mTicksSinceLastAnimUpdate >= (float)ticksPerSecond*0.1;

        if(advanceAnims)
            mTicksSinceLastAnimUpdate = 0;

        for(size_t i = 0; i < mActors.size(); i++)
        {
            mActors[i]->update();

            if(advanceAnims)
                mActors[i]->mFrame = (mActors[i]->mFrame + 1) % mActors[i]->mWalkAnim.get()->mSpriteGroup.animLength();
        }
    }

    Player* World::getPlayer()
    {
        return &mPlayer;
    }
    
    void World::fillRenderState(FARender::RenderState* state)
    {
        state->mObjects.clear();

        for(size_t i = 0; i < mActors.size(); i++)
        {
            size_t frame = mActors[i]->mFrame;

            if(mActors[i]->mPos.mDirection != -1)
            {
                frame += mActors[i]->mPos.mDirection * 8;
            }

            state->mObjects.push_back(boost::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->mWalkAnim, frame, mActors[i]->mPos));
        }
    }
}
