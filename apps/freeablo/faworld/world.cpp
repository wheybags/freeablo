#include "world.h"

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
}
