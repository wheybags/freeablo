#ifndef FAWORLD_LEVEL_H
#define FAWORLD_LEVEL_H

#include <level/level.h>
#include <unordered_map>

#include <enet/enet.h> // TODO: remove

#include <misc/stdhashes.h>
#include "hoverstate.h"
#include "item.h"

namespace FARender
{
    class Renderer;
    class RenderState;
}

namespace FAWorld
{
    class Actor;
    class ItemMap;
    class Tile;

    class GameLevelImpl
    {
    public:
        virtual ~GameLevelImpl() {}
        virtual int32_t width() const = 0;
        virtual int32_t height() const = 0;
        virtual bool isPassable(int x, int y) const = 0;
    };

    class GameLevel :public GameLevelImpl
    {
    public:
        GameLevel(Level::Level level, size_t levelIndex);
        ~GameLevel();

        Level::MinPillar getTile(size_t x, size_t y);

        int32_t width() const;
        int32_t height() const;

        const std::pair<size_t, size_t> upStairsPos() const;
        const std::pair<size_t, size_t> downStairsPos() const;

        void activate(size_t x, size_t y);

        size_t getNextLevel();
        size_t getPreviousLevel();

        void update(bool noclip);

        void actorMapInsert(Actor* actor);
        void actorMapRemove(Actor* actor);
        void actorMapClear();
        void actorMapRefresh();
        virtual bool isPassable(int x, int y) const;

        Actor* getActorAt(int32_t x, int32_t y) const;

        void addActor(Actor* actor);

        void fillRenderState(FARender::RenderState* state, Actor* displayedActor);

        void removeActor(Actor* actor);

        size_t getLevelIndex()
        {
            return mLevelIndex;
        }

        std::string serialiseToString();
        bool isPassableFor(int i, int j, const Actor* actor) const;
        bool dropItem(std::unique_ptr <Item>&& item, const Actor& actor, const Tile &tile);
        static GameLevel* loadFromString(const std::string& data);

        Actor* getActorById(int32_t id);

        void getActors(std::vector<Actor*>& actors);
        HoverState& getHoverState();
        ItemMap &getItemMap();

    private:
        GameLevel();

        Level::Level mLevel;
        size_t mLevelIndex = 0u;

        std::vector<Actor*> mActors;
        std::unordered_map<std::pair<int32_t, int32_t>, Actor*> mActorMap2D;    ///< Map of points to actors.
                                                                    ///< Where an actor straddles two squares, they shall be placed in both.
        friend class FARender::Renderer;
        HoverState mHoverState;
        std::unique_ptr<ItemMap> mItemMap;
    };
}

#endif
