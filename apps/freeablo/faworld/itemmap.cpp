#include "itemmap.h"

#include "item.h"

#include "gamelevel.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../farender/animationplayer.h"
#include "../fasavegame/gameloader.h"

namespace FAWorld
{
    Tile::Tile(FASaveGame::GameLoader& loader)
        : x(loader.load<int32_t>())
        , y(loader.load<int32_t>())
    {}

    void Tile::save(FASaveGame::GameSaver& saver)
    {
        saver.save(x);
        saver.save(y);
    }

    PlacedItemData::PlacedItemData(std::unique_ptr <Item> itemArg, const Tile &tile):
      mItem(std::move(itemArg)),
      mAnimation (new FARender::AnimationPlayer ()),
      mTile (tile)
    {
        mAnimation->playAnimation(mItem->getFlipSpriteGroup(), World::getTicksInPeriod(0.05f), FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
    }

    void PlacedItemData::update()
    {
        mAnimation->update ();
    }

    std::pair<FARender::FASpriteGroup*, int32_t> PlacedItemData::getSpriteFrame()
    {
        return mAnimation->getCurrentFrame();
    }

    bool PlacedItemData::onGround()
    {
        return mAnimation->getCurrentFrame().second == mItem->getFlipSpriteGroup()->getAnimLength() - 1;
    }

    ItemMap::ItemMap(const GameLevel* level)
        : mWidth(level->width())
        , mHeight(level->height())
        , mLevel(level)
    {
    }

    ItemMap::ItemMap(FASaveGame::GameLoader& loader, const GameLevel* level) : ItemMap(level)
    {
        /*uint32_t itemsSize = loader.load<uint32_t>();
        for (uint32_t i; i < itemsSize; i++)
        {
            Tile key(loader);
            mItems[key] =
        }*/
    }

    ItemMap::~ItemMap()
    {
    }

    bool ItemMap::dropItem(std::unique_ptr<Item>&& item, const Actor& actor, const Tile& tile)
    {
        if(!mLevel->isPassableFor(tile.x, tile.y, &actor))
            return false;

        auto it = mItems.find(tile);
        if(it != mItems.end())
            return false;

        Engine::ThreadManager::get()->playSound(item->getFlipSoundPath());
        mItems.emplace (tile, PlacedItemData {std::move(item), tile});
        return true;
    }

    PlacedItemData* ItemMap::getItemAt(const Tile& tile)
    {
      auto it = mItems.find({tile.x, tile.y});
      if(it == mItems.end())
          return nullptr;

      if (!it->second.onGround())
          return nullptr;

      return &it->second;
    }

    std::unique_ptr<Item> ItemMap::takeItemAt(const Tile& tile)
    {
        auto it = mItems.find({tile.x, tile.y});
        if(it == mItems.end())
            return nullptr;

        auto item = std::move (it->second.mItem);
        mItems.erase (it);
        return item;
    }
}
