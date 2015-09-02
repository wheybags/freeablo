#include "world.h"

#include <tuple>
#include <diabloexe/diabloexe.h>
#include "../farender/renderer.h"
#include "../falevelgen/levelgen.h"
#include "../faaudio/audiomanager.h"
#include "../engine/threadmanager.h"
#include "actorstats.h"
#include "monster.h"

namespace FAWorld
{
    World* singletonInstance = NULL;

    World::World(const DiabloExe::DiabloExe& exe) : mDiabloExe(exe)
    {
        assert(singletonInstance == NULL);
        singletonInstance = this;

        mPlayer = new Player();
        mActors.push_back(mPlayer);
        mTicksSinceLastAnimUpdate = 0;
        mCurrentLevel = NULL;
    }

    void World::setStatsObject(ActorStats *stats)
    {
        mPlayer->setStats(stats);

    }

    World::~World()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }

    World* World::get()
    {
        return singletonInstance;
    }

    void World::deleteActorFromWorld(Actor * dead)
    {
        mActorMap2D[dead->mPos.current()] = nullptr;
        mActors.erase(std::remove(mActors.begin(), mActors.end(), dead));
        delete dead;

    }

    void World::generateLevels()
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        mLevels.push_back(Level::Level(Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til",
            "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel", std::make_pair(25,29), std::make_pair(75,68), std::map<size_t, size_t>(), -1, 1));


        for(int32_t i = 1; i < 13; i++)
        {
            mLevels.push_back(FALevelGen::generate(100, 100, i, mDiabloExe, i-1, i+1));
        }
    }

    Level::Level* World::getCurrentLevel()
    {
        return mCurrentLevel;
    }

    size_t World::getCurrentLevelIndex()
    {
        return mCurrentLevelIndex;
    }
    
    void World::setLevel(int32_t levelNum)
    {
        if(levelNum != 0)
            mPlayer->mActorSpriteState.setDungeon(true);
        else
            mPlayer->mActorSpriteState.setDungeon(false);

        if(levelNum >= (int32_t)mLevels.size() || levelNum < 0)
            return;

        clear();

        mCurrentLevel = &mLevels[levelNum];
        mCurrentLevelIndex = levelNum;

        const std::vector<Level::Monster>& monsters = mCurrentLevel->getMonsters();

        for(size_t i = 0; i < monsters.size(); i++)
        {
            DiabloExe::Monster monster =  mDiabloExe.getMonster(monsters[i].name);
            ActorStats * stats = new ActorStats(monster);

            Monster * monsterObj = new Monster(monster, Position(monsters[i].xPos, monsters[i].yPos), stats);
            stats->setActor(monsterObj);
            mActors.push_back(monsterObj);
        }

        actorMapClear();

        // insert actors into 2d map
        for(size_t i = 0; i < mActors.size(); i++)
            actorMapInsert(mActors[i]);

        if(levelNum == 0)
            addNpcs();

        FAAudio::AudioManager::playLevelMusic(levelNum, *Engine::ThreadManager::get());
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

    void World::update(bool noclip)
    {        
        mTicksSinceLastAnimUpdate++;
        mTicksInSecond++;
        bool advanceAnims = mTicksSinceLastAnimUpdate >= (float)ticksPerSecond*0.1;

        /*if(!(mTicksInSecond % ticksPerSecond))
            printf("playerState: %d, %d, %d, %d\n", mPlayer->mAnimPlaying, mPlayer->mPos.mMoving, mPlayer->mAnimState, mPlayer->isDead());*/



        if(advanceAnims)
            mTicksSinceLastAnimUpdate = 0;
                        
        for(size_t i = 0; i < mActors.size(); i++)
        {
            actorMapRemove(mActors[i]);

            mActors[i]->update(noclip);

            if(advanceAnims)
            {

                if(!mActors[i]->mAnimPlaying && !mActors[i]->isDead())
                {
                    mActors[i]->mFrame = fmod((mActors[i]->mFrame + 1), (double)(mActors[i]->getCurrentAnim().animLength));
                }

                else if(mActors[i]->mAnimPlaying && mActors[i]->mFrame <= mActors[i]->getCurrentAnim().animLength-1)
                {
                    mActors[i]->mFrame+=mActors[i]->mAnimStep;
                }

                else if((mActors[i]->mAnimPlaying && mActors[i]->mFrame == mActors[i]->getCurrentAnim().animLength) || mActors[i]->mFrame + mActors[i]->mAnimStep > mActors[i]->getCurrentAnim().animLength)
                {
                    mActors[i]->mAnimPlaying = false;
                    mPlayer->mAnimStep=1;
                }

                else if(!mActors[i]->mAnimPlaying && mActors[i]->mFrame < mActors[i]->getCurrentAnim().animLength-1)
                {
                    mActors[i]->mFrame++;
                }
            }
            
            actorMapInsert(mActors[i]);    
        }

        actorMapClear();
        for(size_t i = 0; i < mActors.size(); i++)
            actorMapInsert(mActors[i]);
    }

    void World::actorMapInsert(Actor* actor)
    {
        mActorMap2D[actor->mPos.current()] = actor;
        if(actor->mPos.mMoving)
            mActorMap2D[actor->mPos.next()] = actor;
    }

    void World::actorMapRemove(Actor* actor)
    {
        if(mActorMap2D[actor->mPos.current()] == actor)
            mActorMap2D.erase(actor->mPos.current());
        if(actor->mPos.mMoving && mActorMap2D[actor->mPos.next()] == actor)
            mActorMap2D.erase(actor->mPos.next());
    }

    void World::actorMapClear()
    {
        mActorMap2D.clear();
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
            state->mObjects.push_back(std::tuple<FARender::FASpriteGroup, size_t, FAWorld::Position>(mActors[i]->getCurrentAnim(), frame, mActors[i]->mPos));
        }
    }
}
