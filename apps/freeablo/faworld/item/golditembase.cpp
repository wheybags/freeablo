#include <cel/celdecoder.h>
#include <farender/renderer.h>
#include <faworld/item/golditem.h>
#include <faworld/item/golditembase.h>

namespace FAWorld
{
    GoldItemBase::GoldItemBase(const DiabloExe::ExeItem& exeItem) : super(exeItem)
    {
        FARender::SpriteLoader& spriteLoader = FARender::Renderer::get()->mSpriteLoader;
        std::vector<Image> itemCursorImages = Cel::CelDecoder(spriteLoader.mGuiSprites.itemCursors.path).decode();

        mInventoryIcon = spriteLoader.getSprite(spriteLoader.mGuiSprites.itemCursors)->getFrame(15);
        mInventoryIconCursor = std::make_unique<Render::Cursor>(itemCursorImages[15], itemCursorImages[15].width() / 2, itemCursorImages[15].height() / 2);

        mInventoryIcon2 = spriteLoader.getSprite(spriteLoader.mGuiSprites.itemCursors)->getFrame(16);
        mInventoryIconCursor2 = std::make_unique<Render::Cursor>(itemCursorImages[16], itemCursorImages[16].width() / 2, itemCursorImages[16].height() / 2);

        mInventoryIcon3 = spriteLoader.getSprite(spriteLoader.mGuiSprites.itemCursors)->getFrame(17);
        mInventoryIconCursor3 = std::make_unique<Render::Cursor>(itemCursorImages[17], itemCursorImages[17].width() / 2, itemCursorImages[17].height() / 2);
    }

    GoldItemBase::~GoldItemBase() = default;

    std::unique_ptr<Item> GoldItemBase::createItem() const { return std::unique_ptr<Item>(new GoldItem(this)); }
}