#include "world.h"

#include <diabloexe/diabloexe.h>

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"

#include "monster.h"

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
    
    void World::setLevel(const Level::Level& level, DiabloExe::DiabloExe& exe)
    {
        const std::vector<Level::Monster>& monsters = level.getMonsters();

        for(size_t i = 0; i < monsters.size(); i++)
            mActors.push_back(new Monster(exe.getMonster(monsters[i].name), Position(monsters[i].xPos, monsters[i].yPos)));
    }

    void World::addNpcs(const DiabloExe::DiabloExe& exe)
    {
        const std::vector<const DiabloExe::Npc*> npcs = exe.getNpcs();

        for(size_t i = 0; i < npcs.size(); i++)
            mActors.push_back(new Actor(npcs[i]->celPath, npcs[i]->celPath, Position(npcs[i]->x, npcs[i]->y)));
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
            size_t frame = mActors[i]->mFrame + mActors[i]->mPos.mDirection * mActors[i]->getCurrentAnim().get()->mSpriteGroup.animLength();
            state->mObjects.push_back(boost::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->getCurrentAnim(), frame, mActors[i]->mPos));
        }
    }
}
