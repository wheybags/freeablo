#ifndef ITEM_MAP_H
#define ITEM_MAP_H

#include <vector>
#include <memory>

#include "misc/helper2d.h"
#include <boost/optional/optional.hpp>
#include "../farender/spritecache.h"

namespace FARender
{
    class AnimationPlayer;
}

namespace Render
{
    class FASpriteGroup;
}

namespace FAWorld
{
    class Actor;
    class GameLevel;
    class Item;

    class ItemMap
    {
        using self = ItemMap;

      public:
        struct ItemData
        {
            std::unique_ptr<Item> item;
            std::unique_ptr<FARender::AnimationPlayer> mAnimation;
            ItemData(std::unique_ptr <Item> itemArg);

            void update();
           std::pair<FARender::FASpriteGroup*, int32_t> getSpriteFrame();
        };

      public:
        ItemMap(const GameLevel* level);
        ~ItemMap ();
        bool dropItem(std::unique_ptr<FAWorld::Item>&& item, const Actor& actor, int i, int j);

      private:
        int32_t mWidth, mHeight;
        std::map<std::pair<int, int>, ItemData> mItems;
        const GameLevel* mLevel;

        friend class GameLevel;
    };
}

#endif
