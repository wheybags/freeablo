#pragma once
#include <map>
#include <memory>
#include <misc/point.h>
#include <optional>
#include <vector>

namespace FARender
{
    class AnimationPlayer;
    class FASpriteGroup;
}

namespace Render
{
    class FASpriteGroup;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class Actor;
    class GameLevel;
    class Item;
    class Tile
    {
    public:
        Misc::Point position;

        Tile() : position(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()) {}
        explicit Tile(Misc::Point position) : position(position) {}
        explicit Tile(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        bool operator==(const Tile& other) const { return position == other.position; }
        bool operator!=(const Tile& other) const { return !(*this == other); }
        bool operator<(const Tile& other) const { return position < other.position; }

        bool isValid() const { return *this != Tile(); }
    };

    class PlacedItemData
    {
    public:
        PlacedItemData(std::unique_ptr<Item> itemArg, const Tile& tile);
        PlacedItemData(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        void update();
        std::pair<FARender::FASpriteGroup*, int32_t> getSpriteFrame();
        Tile getTile() const { return mTile; }
        bool onGround();
        const Item& item() const { return *mItem; }

    private:
        std::unique_ptr<Item> mItem;
        std::unique_ptr<FARender::AnimationPlayer> mAnimation;
        Tile mTile;
        friend class ItemMap;
    };

    class ItemMap
    {
        using self = ItemMap;

    public:
        ItemMap(const GameLevel* level);
        ItemMap(FASaveGame::GameLoader& loader, const GameLevel* level);

        void save(FASaveGame::GameSaver& saver);

        ~ItemMap();
        bool dropItem(std::unique_ptr<FAWorld::Item>&& item, const Actor& actor, const Tile& tile);
        PlacedItemData* getItemAt(const Tile& tile);
        PlacedItemData* getItemAt(const Misc::Point& pos);
        std::unique_ptr<FAWorld::Item> takeItemAt(const Tile& tile);

    private:
        int32_t mWidth, mHeight;
        std::map<Tile, PlacedItemData> mItems;
        const GameLevel* mLevel;

        friend class GameLevel;
    };
}
