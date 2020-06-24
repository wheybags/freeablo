#include "itembaseholder.h"
#include "equipmentitembase.h"
#include "golditembase.h"
#include "usableitembase.h"
#include <diabloexe/baseitem.h>
#include <diabloexe/diabloexe.h>
#include <diabloexe/exemagicitemeffect.h>
#include <faworld/item/item.h>
#include <memory>

namespace FAWorld
{
    ItemBaseHolder::ItemBaseHolder(const DiabloExe::DiabloExe& exe)
    {
        for (const auto& item : exe.getBaseItems())
        {
            if (item.idName == "gold")
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new GoldItemBase(item));
            else if (item.equipType != ItemEquipType::none)
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new EquipmentItemBase(item));
            else if (item.isUsable)
                mAllItemBases[item.idName] = std::unique_ptr<ItemBase>(new UsableItemBase(item));
            else
                mAllItemBases[item.idName] = std::make_unique<ItemBase>(item);
        }

        int32_t goldItemCount = 0;
        for (const auto& pair : mAllItemBases)
        {
            if (dynamic_cast<GoldItemBase*>(pair.second.get()))
                goldItemCount++;
        }

        release_assert(goldItemCount <= 1);

        for (const auto& prefixOrSuffix : exe.getMagicItemEffects())
            mAllItemPrefixSuffixBases[prefixOrSuffix.mIdName] = std::make_unique<ItemPrefixOrSuffixBase>(prefixOrSuffix);
    }

    std::unique_ptr<Item> ItemBaseHolder::createItem(const std::string& baseTypeId) const { return mAllItemBases.at(baseTypeId)->createItem(); }
}
