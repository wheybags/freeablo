#include "itemfactory.h"
#include "misc/random.h"
#include "diabloexe/diabloexe.h"
#include "item.h"
#include "itemenums.h"
#include <boost/make_unique.hpp>

namespace FAWorld
{
    ItemFactory::ItemFactory(const DiabloExe::DiabloExe& exe) : mExe(exe)
    {
        for (int i = 0; i < static_cast<int>(mExe.getBaseItems().size()); ++i)
            mUniqueBaseItemIdToItemId[mExe.getBaseItems()[i].uniqueBaseItemId] = static_cast<ItemId>(i);
    }

    Item ItemFactory::generateBaseItem(ItemId id, const BaseItemGenOptions& /*options*/) const
    {
        Item res;
        res.mIsIdentified = true;
        res.mEmpty = false;
        res.mIsReal = true;
        res.mInvX = 0;
        res.mInvY = 0;
        res.mBaseId = id;
        res.mExe = &mExe;
        auto info = mExe.getBaseItems()[static_cast<int>(id)];
        res.mMaxDurability = res.mCurrentDurability = info.durability;
        res.mArmorClass = Random::randomInRange(info.minArmorClass, info.maxArmorClass);
        return res;
    }

    Item ItemFactory::generateUniqueItem(UniqueItemId id) const
    {
        auto& info = mExe.getUniqueItems()[static_cast<int32_t>(id)];
        auto it = mUniqueBaseItemIdToItemId.find(info.mUniqueBaseItemId);
        if (it == mUniqueBaseItemIdToItemId.end())
            return {};
        auto baseItemId = it->second;
        auto res = generateBaseItem(baseItemId);
        return res;
    }
}
