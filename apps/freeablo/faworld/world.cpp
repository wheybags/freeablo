#include "world.h"

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"

namespace FAWorld
{
    World::World()
    {
        mActors.push_back(&mPlayer);
    }

    void World::update()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            mActors[i]->update();
    }

    Player* World::getPlayer()
    {
        return &mPlayer;
    }
    
    void World::fillRenderState(FARender::RenderState* state)
    {
        state->mObjects.clear();

        for(size_t i = 0; i < mActors.size(); i++)
            state->mObjects.push_back(boost::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->walkAnim, 0, mActors[i]->mPos));
    }
}
