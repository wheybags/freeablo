#include "gamelevel.h"

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "../engine/net/netmanager.h"

#include <diabloexe/diabloexe.h>
#include "monster.h"
#include "world.h"
#include "actorstats.h"

namespace FAWorld
{
    GameLevel::GameLevel(Level::Level level, size_t levelIndex, std::vector<Actor*> actors) : mLevel(level), mLevelIndex(levelIndex)
    {
        for(auto actor : actors)
            actor->setLevel(this);

        actorMapRefresh();
    }

    GameLevel::~GameLevel()
    {
        for(size_t i = 0; i < mActors.size(); i++)
            delete mActors[i];
    }

    Level::MinPillar GameLevel::getTile(size_t x, size_t y)
    {
        return mLevel[x][y];
    }

    size_t GameLevel::width() const
    {
        return mLevel.width();
    }

    size_t GameLevel::height() const
    {
        return mLevel.height();
    }

    const std::pair<size_t,size_t>& GameLevel::upStairsPos() const
    {
        return mLevel.upStairsPos();
    }

    const std::pair<size_t,size_t>& GameLevel::downStairsPos() const
    {
        return mLevel.downStairsPos();
    }

    void GameLevel::activate(size_t x, size_t y)
    {
        mLevel.activate(x, y);
    }

    size_t GameLevel::getNextLevel()
    {
        return mLevel.getNextLevel();
    }

    size_t GameLevel::getPreviousLevel()
    {
        return mLevel.getPreviousLevel();
    }

    void GameLevel::update(bool noclip, size_t tick)
    {
        for(size_t i = 0; i < mActors.size(); i++)
        {
            Actor * actor = mActors[i];

            actorMapRemove(actor);
            actor->update(noclip, tick);
            actorMapInsert(actor);
        }

        actorMapRefresh();
    }

    void GameLevel::actorMapInsert(Actor* actor)
    {
        mActorMap2D[actor->mPos.current()] = actor;
        if(actor->mPos.mMoving)
            mActorMap2D[actor->mPos.next()] = actor;
    }

    void GameLevel::actorMapRemove(Actor* actor)
    {
        if(mActorMap2D[actor->mPos.current()] == actor)
            mActorMap2D.erase(actor->mPos.current());
        if(actor->mPos.mMoving && mActorMap2D[actor->mPos.next()] == actor)
            mActorMap2D.erase(actor->mPos.next());
    }

    void GameLevel::actorMapClear()
    {
        mActorMap2D.clear();
    }

    void GameLevel::actorMapRefresh()
    {
        actorMapClear();
        for(size_t i = 0; i < mActors.size(); i++)
            actorMapInsert(mActors[i]);
    }

    Actor* GameLevel::getActorAt(size_t x, size_t y)
    {
        return mActorMap2D[std::pair<size_t, size_t>(x, y)];
    }

    void GameLevel::addActor(Actor* actor)
    {
        mActors.push_back(actor);
        actorMapInsert(actor);
    }

    void GameLevel::fillRenderState(FARender::RenderState* state)
    {
        state->mObjects.clear();

        for(size_t i = 0; i < mActors.size(); i++)
        {
            size_t frame = mActors[i]->mFrame + mActors[i]->mPos.mDirection * mActors[i]->getCurrentAnim()->getAnimLength();
            state->mObjects.push_back(std::tuple<FARender::FASpriteGroup*, size_t, FAWorld::Position>(mActors[i]->getCurrentAnim(), frame, mActors[i]->mPos));
        }
    }

    void GameLevel::removeActor(Actor* actor)
    {
        for(auto i = mActors.begin(); i != mActors.end(); ++i)
        {
            if(*i == actor)
            {
                mActors.erase(i);
                actorMapRemove(actor);
                return;
            }
        }
        assert(false && "tried to remove actor that isn't in level");
    }

    #pragma pack(1)
    struct GameLevelHeader
    {
        size_t levelIndex;
        size_t contentLength;
    };

    std::string GameLevel::serialiseToString()
    {
        std::string dataSavingTmp;
        boost::iostreams::back_insert_device<std::string> inserter(dataSavingTmp);
        boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
        boost::archive::binary_oarchive oa(s);
        oa & mLevel;
        oa & mLevelIndex;
        s.flush();

        return dataSavingTmp;
    }

    GameLevel* GameLevel::loadFromString(const std::string& data)
    {
        GameLevel* retval = new GameLevel();

        boost::iostreams::basic_array_source<char> device(data.data(), data.size());
        boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
        boost::archive::binary_iarchive ia(s);
        ia & retval->mLevel;
        ia & retval->mLevelIndex;

        return retval;
    }

    Actor* GameLevel::getActorById(int32_t id)
    {
        for(auto actor : mActors)
        {
            if(actor->getId() == id)
                return actor;
        }

        return NULL;
    }

    void GameLevel::getActors(std::vector<Actor*>& actors)
    {
        actors.insert(actors.end(), mActors.begin(), mActors.end());
    }
}
