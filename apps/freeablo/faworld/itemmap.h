#pragma once
#include <faworld/item/item.h>
#include <map>
#include <memory>
#include <misc/simplevec2.h>
#include <optional>
#include <vector>

namespace Render
{
    class SpriteGroup;
}

namespace FARender
{
    class AnimationPlayer;
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

    class PlacedItemData
    {
    public:
        PlacedItemData(std::unique_ptr<Item>&& itemArg, Misc::Point tile);
        explicit PlacedItemData(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        void update();
        std::pair<Render::SpriteGroup*, int32_t> getSpriteFrame();
        Misc::Point getTile() const { return mTile; }
        bool onGround();
        const Item& item() const { return *mItem; }

    private:
        void restoreSprites();

    private:
        std::unique_ptr<Item> mItem;
        std::unique_ptr<FARender::AnimationPlayer> mAnimation;
        Misc::Point mTile;
        friend class ItemMap;
    };

    class ItemMap
    {
        using self = ItemMap;

    public:
        ItemMap(const GameLevel* level);
        ItemMap(FASaveGame::GameLoader& loader, const GameLevel* level);

        void save(FASaveGame::GameSaver& saver) const;

        ~ItemMap();
        bool dropItem(std::unique_ptr<FAWorld::Item>& item, const Actor& actor, Misc::Point tile);
        PlacedItemData* getItemAt(Misc::Point pos);
        std::unique_ptr<FAWorld::Item> takeItemAt(Misc::Point tile);

    private:
        int32_t mWidth, mHeight;
        std::map<Misc::Point, PlacedItemData> mItems;
        const GameLevel* mLevel;

        friend class GameLevel;
    };
}
