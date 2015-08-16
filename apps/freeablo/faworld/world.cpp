#include "world.h"

#include <diabloexe/diabloexe.h>

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"

#include "monster.h"

namespace FAWorld
{
    World::World(ActorStats * stats)
    {
        mPlayer = new Player(stats);
        mActors.push_back(mPlayer);
        mTicksSinceLastAnimUpdate = 0;
        mLevel = NULL;
    }

    World::~World()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }
    
    void World::setLevel(Level::Level& level, const DiabloExe::DiabloExe& exe)
    {
        const std::vector<Level::Monster>& monsters = level.getMonsters();

        for(size_t i = 0; i < monsters.size(); i++)
            mActors.push_back(new Monster(exe.getMonster(monsters[i].name), Position(monsters[i].xPos, monsters[i].yPos)));

        mLevel = &level;

        mActorMap2D.resize(level.width()*level.height());
        actorMapClear();

        // insert actors into 2d map
        for(size_t i = 0; i < mActors.size(); i++)
            actorMapInsert(mActors[i]);    
    }

    void World::addNpcs(const DiabloExe::DiabloExe& exe)
    {
        const std::vector<const DiabloExe::Npc*> npcs = exe.getNpcs();

        for(size_t i = 0; i < npcs.size(); i++)
        {
            Actor* actor = new Actor(npcs[i]->celPath, npcs[i]->celPath, Position(npcs[i]->x, npcs[i]->y, npcs[i]->rotation));
            actorMapInsert(actor);
            mActors.push_back(actor);
        }
    }

    Actor* World::getActorAt(size_t x, size_t y)
    {
        return mActorMap2D[(y*mLevel->width())+x];
    }

    void World::clear()
    {
        for(size_t i = 0; i < mActors.size(); i++)
        {
            if(mActors[i] != mPlayer)
                delete mActors[i];
        }

        mActors.clear();
        mActors.push_back(mPlayer);
    }

    void World::update()
    {
        mTicksSinceLastAnimUpdate++;

        bool advanceAnims = mTicksSinceLastAnimUpdate >= (float)ticksPerSecond*0.1;

        if(advanceAnims)
            mTicksSinceLastAnimUpdate = 0;
        
        actorMapClear();
                
        for(size_t i = 0; i < mActors.size(); i++)
        {
            mActors[i]->update();

            if(advanceAnims)
                mActors[i]->mFrame = (mActors[i]->mFrame + 1) % mActors[i]->getCurrentAnim().animLength;
            
            actorMapInsert(mActors[i]);    
        }
    }

    void World::actorMapClear()
    {
        for(size_t i = 0; i < mActorMap2D.size(); i++)
            mActorMap2D[i] = NULL;
    }

    void World::actorMapInsert(Actor* actor)
    {
        mActorMap2D[actor->mPos.current().second*mLevel->width()+actor->mPos.current().first] = actor;
        if(actor->mPos.mMoving)
            mActorMap2D[actor->mPos.next().second*mLevel->width()+actor->mPos.next().first] = actor;
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
            size_t frame = mActors[i]->mFrame + mActors[i]->mPos.mDirection * mActors[i]->getCurrentAnim().animLength;
            state->mObjects.push_back(boost::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->getCurrentAnim(), frame, mActors[i]->mPos));
        }
    }
}
