#include "usableitembase.h"
#include <faworld/item/usableitem.h>

namespace FAWorld
{
    UsableItemBase::UsableItemBase(const DiabloExe::ExeItem& exeItem) : super(exeItem) {}

    bool UsableItemBase::isBeltEquippable() const { return mSize == Vec2i(1, 1); }

    std::unique_ptr<Item> UsableItemBase::createItem() const { return std::unique_ptr<Item>(new UsableItem(this)); }
}