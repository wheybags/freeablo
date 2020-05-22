#include "itemfactory.h"
#include "diabloexe/diabloexe.h"
#include "item.h"
#include "itemenums.h"
#include <random/random.h>

namespace FAWorld
{
    ItemFilter::Callback ItemFilter::maxQLvl(int32_t value)
    {
        return [value](const DiabloExe::BaseItem& item) { return static_cast<int32_t>(item.qualityLevel) <= value; };
    }

    ItemFilter::Callback ItemFilter::sellableGriswoldBasic()
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
        const DiabloExe::UniqueItem& info = mExe.getUniqueItems()[static_cast<int32_t>(id)];
        auto it = mUniqueBaseItemIdToItemId.find(info.mUniqueBaseItemId);
        if (it == mUniqueBaseItemIdToItemId.end())
            return {};
        ItemId baseItemId = it->second;
        return generateBaseItem(baseItemId);
    }

    ItemId ItemFactory::randomItemId(const ItemFilter::Callback& filter) const
    {
        static std::vector<ItemId> pool;
        pool.clear();
        for (auto id : enum_range<ItemId>())
        {
            const DiabloExe::BaseItem& info = getInfo(id);
            if (filter(info))
                continue;
            for (int32_t i = 0; i < static_cast<int32_t>(info.dropRate); ++i)
                pool.push_back(id);
        }
        return pool[mRng.randomInRange(0, pool.size() - 1)];
    }

    const DiabloExe::BaseItem& ItemFactory::getInfo(ItemId id) const { return mExe.getBaseItems().at(static_cast<int>(id)); }
}
