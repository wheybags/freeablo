#include "itembase.h"
#include "item.h"
#include <cel/celdecoder.h>
#include <diabloexe/baseitem.h>
#include <farender/renderer.h>

namespace FAWorld
{
    ItemBase::ItemBase(const DiabloExe::ExeItem& exeItem)
        : mId(exeItem.idName), mType(exeItem.type), mClass(exeItem.itemClass), mName(exeItem.name), mShortName(exeItem.shortName),
          mSize(exeItem.invSizeX, exeItem.invSizeY), mPrice(exeItem.price), mQualityLevel(exeItem.qualityLevel), mDropRate(exeItem.dropRate),
          mDropItemSoundPath(exeItem.dropItemSoundPath), mInventoryPlaceItemSoundPath(exeItem.invPlaceItemSoundPath)
    {
        FARender::SpriteLoader& spriteLoader = FARender::Renderer::get()->mSpriteLoader;
        mDropItemAnimation = spriteLoader.getSprite(spriteLoader.mItemDrops[mId]);

        Render::SpriteGroup* itemIcons = spriteLoader.getSprite(spriteLoader.mGuiSprites.itemCursors);
        int32_t itemIconFrame = exeItem.invGraphicsId + 11;

        // this check is only here for gold, it has a hardcoded graphics id instead
        if (itemIconFrame >= 0 && itemIconFrame < itemIcons->size())
        {
            mInventoryIcon = itemIcons->getFrame(itemIconFrame);
            static std::vector<Image> itemCursorImages = Cel::CelDecoder(spriteLoader.mGuiSprites.itemCursors.path).decode();

            const Image& cursorImage = itemCursorImages[itemIconFrame];
            mInventoryIconCursor = std::make_unique<Render::Cursor>(cursorImage, cursorImage.width() / 2, cursorImage.height() / 2);
        }
    }

    ItemBase::~ItemBase() = default;

    std::unique_ptr<Item> ItemBase::createItem() const { return std::make_unique<Item>(this); }
}