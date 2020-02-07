#include "itemfactory.h"
#include "diabloexe/diabloexe.h"
#include "item.h"
#include "itemenums.h"
#include <random/random.h>

namespace FAWorld
{
    std::function<bool(const DiabloExe::BaseItem& item)> ItemFilter::maxQLvl(int32_t value)
    {
        return [value](const DiabloExe::BaseItem& item) { return static_cast<int32_t>(item.qualityLevel) <= value; };
    }

    std::function<bool(const DiabloExe::BaseItem& item)> ItemFilter::sellableGriswoldBasic()
    {
        return [](const DiabloExe::BaseItem& item) {
            static const auto excludedTypes = {ItemType::misc, ItemType::gold, ItemType::staff, ItemType::ring, ItemType::amulet};
            return std::count(excludedTypes.begin(), excludedTypes.end(), static_cast<ItemType>(item.type)) == 0;
        };
    }

    ItemFactory::ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng) : mExe(exe), mRng(rng)
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
        auto info = getInfo(id);
        res.mMaxDurability = res.mCurrentDurability = info.durability;
        res.mArmorClass = mRng.randomInRange(info.minArmorClass, info.maxArmorClass);
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

    const DiabloExe::BaseItem& ItemFactory::getInfo(ItemId id) const { return mExe.getBaseItems().at(static_cast<int>(id)); }
}
