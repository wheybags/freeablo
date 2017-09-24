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
#include "itemmap.h"

namespace FAWorld
{
    GameLevel::GameLevel(Level::Level level, size_t levelIndex) : mLevel(level), mLevelIndex(levelIndex),
       mItemMap (new ItemMap (this))
    {
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

    int32_t GameLevel::width() const
    {
        return mLevel.width();
    }

    int32_t GameLevel::height() const
    {
        return mLevel.height();
    }

    const std::pair<size_t,size_t> GameLevel::upStairsPos() const
    {
        return mLevel.upStairsPos();
    }

    const std::pair<size_t,size_t> GameLevel::downStairsPos() const
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

    void GameLevel::update(bool noclip)
    {
        for(size_t i = 0; i < mActors.size(); i++)
        {
            Actor * actor = mActors[i];

            actorMapRemove(actor);
            actor->update(noclip);
            actorMapInsert(actor);
        }

        actorMapRefresh();

        for (auto &p : mItemMap->mItems)
            {
                p.second.update ();
            }
    }

    void GameLevel::actorMapInsert(Actor* actor)
    {
        mActorMap2D[actor->getPos().current()] = actor;
        if(actor->getPos().isMoving())
            mActorMap2D[actor->getPos().next()] = actor;
    }

    void GameLevel::actorMapRemove(Actor* actor)
    {
        if(mActorMap2D[actor->getPos().current()] == actor)
            mActorMap2D.erase(actor->getPos().current());
        if(actor->getPos().isMoving() && mActorMap2D[actor->getPos().next()] == actor)
            mActorMap2D.erase(actor->getPos().next());
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

    bool GameLevel::isPassable(int x, int y) const
    {
        if (x > 0 && x < width() && y > 0 && y < height() && !mLevel[x][y].passable())
            return false;

        FAWorld::Actor* actor = getActorAt(x, y);
        return actor == NULL || actor->isPassable();
    }

    Actor* GameLevel::getActorAt(int32_t x, int32_t y) const
    {
        auto it = mActorMap2D.find(std::pair<int32_t, int32_t>(x, y));
        if (it == mActorMap2D.end())
            return nullptr;

        return it->second;
    }

    void GameLevel::addActor(Actor* actor)
    {
        mActors.push_back(actor);
        actorMapInsert(actor);
    }

    static Cel::Colour friendHoverColor () { return {180, 110, 110, true};}
    static Cel::Colour enemyHoverColor () { return {164, 46, 46, true}; }
    static Cel::Colour itemHoverColor () { return {185, 170, 119, true}; }

    void GameLevel::fillRenderState(FARender::RenderState* state, Actor* displayedActor)
    {
        state->mObjects.clear();
        state->mItems.clear ();

        for(size_t i = 0; i < mActors.size(); i++)
        {
            auto tmp = mActors[i]->getAnimationManager().getCurrentRealFrame();

            FARender::FASpriteGroup* sprite = tmp.first;
            int32_t frame = tmp.second;
            boost::optional<Cel::Colour> hoverColor;
            if (mHoverState.isActorHovered(mActors[i]->getId()))
               hoverColor = mActors[i]->isEnemy (displayedActor) ? enemyHoverColor() : friendHoverColor();
            if (!sprite)
                sprite = FARender::getDefaultSprite();


            // offset the sprite for the current direction of the actor
            frame += mActors[i]->getPos().getDirection() * sprite->getAnimLength();

            state->mObjects.push_back({sprite, static_cast<uint32_t> (frame), mActors[i]->getPos(), hoverColor});

            for (auto &p : mItemMap->mItems)
               {
                   auto sf = p.second.getSpriteFrame();
                   FARender::ObjectToRender o;
                   o.spriteGroup = sf.first;
                   o.frame = sf.second;
                   o.position = {p.first.x, p.first.y};
                   if (mHoverState.isItemHovered(p.first))
                       o.hoverColor = itemHoverColor ();
                   state->mItems.push_back(o);
               }
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

    bool GameLevel::isPassableFor(int x, int y, const Actor *actor) const
     {
       auto actorAtPos = getActorAt (x, y);
       return mLevel[x][y].passable() && (actorAtPos == nullptr || actorAtPos == actor || actorAtPos->isPassable());
     }

    bool GameLevel::dropItem(std::unique_ptr <Item>&& item, const Actor& actor, const Tile &tile)
    {
        return mItemMap->dropItem (move (item), actor, tile);
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

    HoverState& GameLevel::getHoverState() {
       return mHoverState;
    }

    GameLevel::GameLevel()
    {
    }

    ItemMap& GameLevel::getItemMap()
    {
        return *mItemMap;
    }
}
