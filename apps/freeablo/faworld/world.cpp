#include "world.h"

#include <diabloexe/diabloexe.h>

#include <boost/tuple/tuple.hpp>

#include "../farender/renderer.h"
#include "../falevelgen/levelgen.h"

#include "monster.h"

namespace FAWorld
{
    World::World(const DiabloExe::DiabloExe& exe) : mDiabloExe(exe)
    {
        mPlayer = new Player();
        mActors.push_back(mPlayer);
        mTicksSinceLastAnimUpdate = 0;
        mCurrentLevel = NULL;
    }

    World::~World()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }

    void World::generateLevels()
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        mLevels.push_back(Level::Level(Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til",
            "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel", std::make_pair(25,29), std::make_pair(75,68), std::map<size_t, size_t>()));


        for(size_t i = 1; i < 13; i++)
        {
            mLevels.push_back(FALevelGen::generate(100, 100, i, mDiabloExe));
        }
    }

    Level::Level* World::getCurrentLevel()
    {
        return mCurrentLevel;
    }
    
    void World::setLevel(size_t levelNum)
    {
        clear();

        mCurrentLevel = &mLevels[levelNum];

        const std::vector<Level::Monster>& monsters = mCurrentLevel->getMonsters();

        for(size_t i = 0; i < monsters.size(); i++)
            mActors.push_back(new Monster(mDiabloExe.getMonster(monsters[i].name), Position(monsters[i].xPos, monsters[i].yPos)));

        actorMapClear();

        // insert actors into 2d map
        for(size_t i = 0; i < mActors.size(); i++)
            actorMapInsert(mActors[i]);

        if(levelNum == 0)
            addNpcs();
    }

    void World::addNpcs()
    {
        const std::vector<const DiabloExe::Npc*> npcs = mDiabloExe.getNpcs();

        for(size_t i = 0; i < npcs.size(); i++)
        {
            Actor* actor = new Actor(npcs[i]->celPath, npcs[i]->celPath, Position(npcs[i]->x, npcs[i]->y, npcs[i]->rotation));
            actorMapInsert(actor);
            mActors.push_back(actor);
        }
    }

    Actor* World::getActorAt(size_t x, size_t y)
    {
        return mActorMap2D[std::pair<size_t, size_t>(x, y)];
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
        mActorMap2D.clear();
    }

    void World::actorMapInsert(Actor* actor)
    {
        mActorMap2D[actor->mPos.current()] = actor;
        if(actor->mPos.mMoving)
            mActorMap2D[actor->mPos.next()] = actor;
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
