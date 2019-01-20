#pragma once

#include "hoverstate.h"
#include "itemmap.h" // TODO: remove, only included for the Tile type
#include "misc/point.h"
#include <level/level.h>
#include <misc/stdhashes.h>
#include <unordered_map>

namespace FARender
{
    class Renderer;
    class RenderState;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class Actor;

    class ItemMap;

    class Tile;

    class World;

    class GameLevelImpl
    {
    public:
        virtual ~GameLevelImpl() {}

        virtual int32_t width() const = 0;

        virtual int32_t height() const = 0;

        virtual bool isPassable(const Misc::Point& point, const FAWorld::Actor* forActor) const = 0;
    };

    class GameLevel : public GameLevelImpl
    {
    public:
        GameLevel(World& world, Level::Level&& level, size_t levelIndex);

        GameLevel(World& world, FASaveGame::GameLoader& gameLoader);

        void save(FASaveGame::GameSaver& gameSaver);

        ~GameLevel();

        Level::MinPillar getTile(const Misc::Point& point) const;

        int32_t width() const;

        int32_t height() const;

        const Misc::Point upStairsPos() const;

        const Misc::Point downStairsPos() const;

        void activate(const Misc::Point& point);

        int32_t getNextLevel();

        int32_t getPreviousLevel();

        void update(bool noclip);

        void insertActor(Actor* actor);
        void actorMapInsert(Actor* actor);

        void actorMapRemove(Actor* actor);

        void actorMapClear();

        void actorMapRefresh();

        Misc::Point getFreeSpotNear(Misc::Point point, int32_t radius = 100) const;

        virtual bool isPassable(const Misc::Point& point, const FAWorld::Actor* forActor) const;

        Actor* getActorAt(const Misc::Point& point) const;

        void fillRenderState(FARender::RenderState* state, Actor* displayedActor, const HoverStatus& hoverStatus);

        void removeActor(Actor* actor);

        int32_t getLevelIndex() { return mLevelIndex; }

        bool dropItem(std::unique_ptr<Item>&& item, const Actor& actor, const Tile& tile);
        bool dropItemClosestEmptyTile(Item& item, const Actor& actor, const Misc::Point& position, Misc::Direction direction);

        Actor* getActorById(int32_t id);

        void getActors(std::vector<Actor*>& actors);

        ItemMap& getItemMap();

        bool isTown() const;

        World* getWorld() { return &mWorld; }

    private:
        GameLevel(World& world);

        World& mWorld;
        Level::Level mLevel;
        int32_t mLevelIndex = 0;

        std::vector<Actor*> mActors;
        std::unordered_map<Misc::Point, Actor*> mActorMap2D; ///< Map of points to actors.
        ///< Where an actor straddles two squares, they shall be placed in both.
        friend class FARender::Renderer;

        std::unique_ptr<ItemMap> mItemMap;
    };
}
