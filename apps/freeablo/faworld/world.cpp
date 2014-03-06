#include "world.h"

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"

namespace FAWorld
{
    World::World()
    {
        mPlayer = new Player();
        mActors.push_back(mPlayer);
        mTicksSinceLastAnimUpdate = 0;
    }

    World::~World()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }
    
    void World::setLevel(const Level::Level& level)
    {
        const std::vector<Level::Monster>& monsters = level.getMonsters();

        for(size_t i = 0; i < monsters.size(); i++)
            mActors.push_back(new Actor("monsters/zombie/zombiew.cl2", "monsters/zombie/zombien.cl2", Position(monsters[i].xPos, monsters[i].yPos)));
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
                mActors[i]->mFrame = (mActors[i]->mFrame + 1) % mActors[i]->getCurrentAnim().get()->mSpriteGroup.animLength();
        }
    }

    Player* World::getPlayer()
    {
        return mPlayer;
    }
    
    void World::fillRenderState(FARender::RenderState* state)
    {
        state->mObjects.clear();

        for(size_t i = 0; i < mActors.size(); i++)
        {
            size_t frame = mActors[i]->mFrame;

            if(mActors[i]->mPos.mDirection != -1)
            {
                frame += mActors[i]->mPos.mDirection * mActors[i]->getCurrentAnim().get()->mSpriteGroup.animLength();
            }

            state->mObjects.push_back(boost::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->getCurrentAnim(), frame, mActors[i]->mPos));
        }
    }
}
