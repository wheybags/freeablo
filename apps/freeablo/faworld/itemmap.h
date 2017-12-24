
#pragma once

#include <map>
#include <memory>
#include <vector>

#include "misc/helper2d.h"
#include <boost/optional/optional.hpp>

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
        int32_t x;
        int32_t y;

        Tile(int32_t x, int32_t y) : x(x), y(y) {}
        Tile() : x(0), y(0) {}
        Tile(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        bool operator==(const Tile& other) const { return std::tie(x, y) == std::tie(other.x, other.y); }
        bool operator<(const Tile& other) const { return std::tie(x, y) < std::tie(other.x, other.y); }
    };

    class PlacedItemData
    {
    public:
        PlacedItemData(std::unique_ptr<Item> itemArg, const Tile& tile);

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

    class ItemTarget
    {
    public:
        enum class ActionType
        {
            autoEquip,
            toCursor,
        };
        ActionType action;
        PlacedItemData* item;
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
        std::unique_ptr<FAWorld::Item> takeItemAt(const Tile& tile);

    private:
        int32_t mWidth, mHeight;
        std::map<Tile, PlacedItemData> mItems;
        const GameLevel* mLevel;

        friend class GameLevel;
    };
}

