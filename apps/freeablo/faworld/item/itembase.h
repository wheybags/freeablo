#pragma once
#include <memory>
#include <misc/commonenums.h>
#include <misc/simplevec2.h>
#include <render/spritegroup.h>

namespace Render
{
    class Cursor;
}

namespace DiabloExe
{
    class ExeItem;
}

namespace FAWorld
{
    class Item;

    class ItemBase
    {
    public:
        explicit ItemBase(const DiabloExe::ExeItem& exeItem);
        virtual ~ItemBase();

        virtual std::unique_ptr<Item> createItem() const;
        virtual ItemEquipType getEquipType() const { return ItemEquipType::none; }

    public:
        std::string mId;
        ItemType mType = ItemType::none;
        ItemClass mClass = ItemClass::none;

        std::string mName;
        std::string mShortName;

        Vec2i mSize;
        int32_t mPrice = 0;
        int32_t mQualityLevel = 0;
        int32_t mDropRate = 0;

        Render::SpriteGroup* mDropItemAnimation = nullptr;
        const Render::TextureReference* mInventoryIcon = nullptr;
        std::unique_ptr<Render::Cursor> mInventoryIconCursor;

        std::string mDropItemSoundPath;
        std::string mInventoryPlaceItemSoundPath;
    };
}
