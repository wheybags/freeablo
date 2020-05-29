#include "itembaseholder.h"
#include "equipmentitembase.h"
#include "usableitembase.h"
#include <diabloexe/diabloexe.h>
#include <faworld/item/item.h>
#include <memory>

namespace FAWorld
{
    ItemBaseHolder::ItemBaseHolder(const DiabloExe::DiabloExe& exe)
    {
        for (const auto& item : exe.getBaseItems())
        {
            if (item.equipType != ItemEquipType::none)
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new EquipmentItemBase(item));
            else if (item.isUsable)
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new UsableItemBase(item));
            else
                mAllItemBases[item.idName] = std::make_unique<ItemBase>(item);
        }
    }

    std::unique_ptr<Item2> ItemBaseHolder::createItem(const std::string& baseTypeId) const { return mAllItemBases.at(baseTypeId)->createItem(); }
}
