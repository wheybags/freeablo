#include "itemmap.h"

#include "item.h"

#include "gamelevel.h"
#include "world.h"
#include "../engine/threadmanager.h"
#include "../farender/animationplayer.h"

FAWorld::PlacedItemData::PlacedItemData(std::unique_ptr <Item> itemArg, const Tile &tile):
  mItem(std::move(itemArg)),
  mAnimation (new FARender::AnimationPlayer ()),
  mTile (tile)
{
    mAnimation->playAnimation(mItem->getFlipSpriteGroup(), FAWorld::World::getTicksInPeriod(0.5f), FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
}

void FAWorld::PlacedItemData::update()
{
    mAnimation->update ();
}

std::pair<FARender::FASpriteGroup*, int32_t> FAWorld::PlacedItemData::getSpriteFrame()
{
    return mAnimation->getCurrentFrame();
}

bool FAWorld::PlacedItemData::onGround()
{
    return mAnimation->getCurrentFrame().second == mItem->getFlipSpriteGroup()->getAnimLength() - 1;
}

FAWorld::ItemMap::ItemMap(const GameLevel* level) : mWidth(level->width()), mHeight(level->height()), mLevel(level)
{
}

FAWorld::ItemMap::~ItemMap()
{
}

bool FAWorld::ItemMap::dropItem(std::unique_ptr<FAWorld::Item>&& item, const FAWorld::Actor& actor, const FAWorld::Tile& tile)
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

FAWorld::PlacedItemData* FAWorld::ItemMap::getItemAt(const Tile& tile)
{
  auto it = mItems.find({tile.x, tile.y});
  if(it == mItems.end())
      return nullptr;

  if (!it->second.onGround())
      return nullptr;

  return &it->second;
}

std::unique_ptr<FAWorld::Item> FAWorld::ItemMap::takeItemAt(const Tile& tile)
{
    auto it = mItems.find({tile.x, tile.y});
    if(it == mItems.end())
        return nullptr;

    auto item = std::move (it->second.mItem);
    mItems.erase (it);
    return item;
}

