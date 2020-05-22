#include "itembaseholder.h"
#include "equipmentitembase.h"
#include "miscitembase.h"
#include <diabloexe/diabloexe.h>

namespace FAWorld
{
    ItemBaseHolder::ItemBaseHolder(const DiabloExe::DiabloExe& exe)
    {
        for (const auto& item : exe.getBaseItems())
        {
            if (item.equipType != ItemEquipType::none)
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new EquipmentItemBase(item));
            else
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new MiscItemBase(item));
        }
    }
}
