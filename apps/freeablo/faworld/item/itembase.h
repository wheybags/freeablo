#pragma once
#include <diabloexe/baseitem.h>
#include <memory>
#include <misc/simplevec2.h>
#include <render/spritegroup.h>

namespace FAWorld
{
    class Item;

    class ItemBase
    {
    public:
        explicit ItemBase(const DiabloExe::ExeItem& exeItem);
        virtual ~ItemBase() = default;

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

        Render::SpriteGroup* mDropItemAnimation = nullptr;
        int32_t mInventoryGraphicsId = -1;

        std::string mDropItemSoundPath;
        std::string mInventoryPlaceItemSoundPath;
    };
}
