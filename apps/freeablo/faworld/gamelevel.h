#pragma once
#include "hoverstate.h"
#include "itemmap.h" // TODO: remove, only included for the Tile type
#include <functional>
#include <level/level.h>
#include <misc/stdhashes.h>
#include <unordered_map>
#include <unordered_set>

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

    namespace Missile
    {
        class Missile;
        class MissileGraphic;
    }

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

        void save(FASaveGame::GameSaver& gameSaver) const;

        ~GameLevel();

        Level::MinPillar getTile(const Misc::Point& point) const;

        int32_t width() const;

        int32_t height() const;

        const Misc::Point upStairsPos() const;

        const Misc::Point downStairsPos() const;

        bool isDoor(const Misc::Point& point) const;
        bool activateDoor(const Misc::Point& point);

        int32_t getNextLevel();

        int32_t getPreviousLevel();

        void update(bool noclip);

        void insertActor(Actor* actor);
        void actorMapInsert(Actor* actor);

        void actorMapRemove(const Actor* actor, Misc::Point point);

        void actorMapClear();

        void actorMapRefresh();

        // TODO: Remove the additionalConstraints parameter, it is currently only used as a bit of a hack to not
        //  place a player on a town portal when teleporting (see https://github.com/wheybags/freeablo/issues/478)
        Misc::Point getFreeSpotNear(Misc::Point point,
                                    int32_t radius = std::numeric_limits<int32_t>::max(),
                                    const std::function<bool(const Misc::Point& point)>& additionalConstraints = nullptr) const;

        virtual bool isPassable(const Misc::Point& point, const FAWorld::Actor* forActor) const;

        Actor* getActorAt(const Misc::Point& point) const;

        void fillRenderState(FARender::RenderState* state, Actor* displayedActor, const HoverStatus& hoverStatus);

        void removeActor(Actor* actor);

        int32_t getLevelIndex() const { return mLevelIndex; }

        bool dropItem(std::unique_ptr<Item>&& item, const Actor& actor, Misc::Point tile);
        bool dropItemClosestEmptyTile(Item& item, const Actor& actor, const Misc::Point& position, Misc::Direction direction);

        Actor* getActorById(int32_t id);

        ItemMap& getItemMap();

        bool isTown() const;

        World* getWorld() { return &mWorld; }

        // This list avoids having to check every actor in world to find missile graphics on a level.
        // It is not saved, items are added/removed in MissileGraphic constructor/destructor.
        // This is currently only intended for rendering so order is unimportant, hence using std::unordered_set
        // and not saving/loading. Since order is not maintained this should not use be used for game logic!
        std::unordered_set<Missile::MissileGraphic*> mMissileGraphics;

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
