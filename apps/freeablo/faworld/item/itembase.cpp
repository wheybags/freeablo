#include "itembase.h"
#include "../../farender/renderer.h"
#include "item.h"

namespace FAWorld
{
    ItemBase::ItemBase(const DiabloExe::ExeItem& exeItem)
        : mId(exeItem.idName), mType(exeItem.type), mClass(exeItem.itemClass), mName(exeItem.name), mShortName(exeItem.shortName),
          mSize(exeItem.invSizeX, exeItem.invSizeY), mPrice(exeItem.price), mInventoryGraphicsId(exeItem.invGraphicsId + 11),
          mDropItemSoundPath(exeItem.dropItemSoundPath), mInventoryPlaceItemSoundPath(exeItem.invPlaceItemSoundPath)
    {
        FARender::SpriteLoader& spriteLoader = FARender::Renderer::get()->mSpriteLoader;
        mDropItemAnimation = spriteLoader.getSprite(spriteLoader.mItemDrops[mId]);
    }

    std::unique_ptr<Item> ItemBase::createItem() const { return std::make_unique<Item>(this); }
}