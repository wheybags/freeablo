#include "itemmap.h"
#include "../engine/threadmanager.h"
#include "../farender/animationplayer.h"
#include "gamelevel.h"
#include "item.h"
#include "world.h"
#include <memory>
#include <render/spritegroup.h>

namespace FAWorld
{
    PlacedItemData::PlacedItemData(std::unique_ptr<Item> itemArg, Misc::Point tile)
        : mItem(std::move(itemArg)), mAnimation(new FARender::AnimationPlayer()), mTile(tile)
    {
        mAnimation->playAnimation(mItem->getFlipSpriteGroup(), World::getTicksInPeriod("0.05"), FARender::AnimationPlayer::AnimationType::FreezeAtEnd);
    }

    PlacedItemData::PlacedItemData(FASaveGame::GameLoader& loader)
    {
        mItem = std::make_unique<Item>();
        mItem->load(loader);
        mAnimation = std::make_unique<FARender::AnimationPlayer>();
        mAnimation->load(loader);
        mTile = Misc::Point(loader);

        restoreSprites();
    }

    void PlacedItemData::save(FASaveGame::GameSaver& saver) const
    {
        mItem->save(saver);
        mAnimation->save(saver);
        mTile.save(saver);
    }

    void PlacedItemData::update() { mAnimation->update(); }

    std::pair<Render::SpriteGroup*, int32_t> PlacedItemData::getSpriteFrame() { return mAnimation->getCurrentFrame(); }

    bool PlacedItemData::onGround() { return mAnimation->getCurrentFrame().second == mItem->getFlipSpriteGroup()->getAnimLength() - 1; }

    void PlacedItemData::restoreSprites()
    {
        mAnimation->replaceAnimation(mItem->getFlipSpriteGroup());
        mAnimation->animationRestoredAfterSave = true;
    }

    ItemMap::ItemMap(const GameLevel* level) : mWidth(level->width()), mHeight(level->height()), mLevel(level) {}

    ItemMap::ItemMap(FASaveGame::GameLoader& loader, const GameLevel* level) : ItemMap(level)
    {
        uint32_t itemsSize = loader.load<uint32_t>();
        for (uint32_t i = 0; i < itemsSize; i++)
        {
            Misc::Point key(loader);
            mItems.emplace(key, PlacedItemData(loader));
        }
    }

    void ItemMap::save(FASaveGame::GameSaver& saver) const
    {
        uint32_t itemsSize = uint32_t(mItems.size());
        saver.save(itemsSize);
        for (const auto& pair : mItems)
        {
            pair.first.save(saver);
            pair.second.save(saver);
        }
    }

    ItemMap::~ItemMap() {}

    bool ItemMap::dropItem(std::unique_ptr<Item>&& item, const Actor& actor, Misc::Point tile)
    {
        if (!mLevel->isPassable(tile, &actor))
            return false;

        auto it = mItems.find(tile);
        if (it != mItems.end())
            return false;

        Engine::ThreadManager::get()->playSound(item->getFlipSoundPath());
        mItems.emplace(tile, PlacedItemData{std::move(item), tile});
        return true;
    }

    PlacedItemData* ItemMap::getItemAt(Misc::Point pos)
    {
        auto it = mItems.find(pos);
        if (it == mItems.end())
            return nullptr;

        if (!it->second.onGround())
            return nullptr;

        return &it->second;
    }

    std::unique_ptr<Item> ItemMap::takeItemAt(Misc::Point tile)
    {
        auto it = mItems.find(tile);
        if (it == mItems.end())
            return nullptr;

        if (!it->second.onGround())
            return nullptr;

        auto item = std::move(it->second.mItem);
        mItems.erase(it);
        return item;
    }
}
