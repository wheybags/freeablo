#include "itemfactory.h"
#include "diabloexe/diabloexe.h"
#include <engine/enginemain.h>
#include <fasavegame/gameloader.h>
#include <faworld/item/equipmentitem.h>
#include <faworld/item/itemprefixorsuffix.h>
#include <random/random.h>

namespace FAWorld
{
    ItemFilter::Callback ItemFilter::maxQLvl(int32_t value)
    {
        return [value](const ItemBase& base) { return base.mQualityLevel <= value; };
    }

    ItemFilter::Callback ItemFilter::sellableGriswoldBasic()
    {
        return [](const ItemBase& base) {
            static const auto excludedTypes = {ItemType::misc, ItemType::gold, ItemType::staff, ItemType::ring, ItemType::amulet};
            return std::count(excludedTypes.begin(), excludedTypes.end(), base.mType) == 0;
        };
    }

    ItemFactory::ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng) : mItemBaseHolder(exe), mExe(exe), mRng(rng) {}

    std::unique_ptr<Item> ItemFactory::generateBaseItem(const std::string& id) const
    {
        std::unique_ptr<Item> newItem = mItemBaseHolder.createItem(id);
        newItem->init();
        return newItem;
    }

    const ItemBase* ItemFactory::randomItemBase(const ItemFilter::Callback& filter) const
    {
        static std::vector<const ItemBase*> pool;
        pool.clear();
        for (const auto& pair : mItemBaseHolder.getAllItemBases())
        {
            const ItemBase* base = pair.second.get();
            if (!filter(*base))
                continue;

            for (int32_t i = 0; i < base->mDropRate; ++i)
                pool.push_back(base);
        }

        if (!pool.empty())
            return pool[mRng.randomInRange(0, pool.size() - 1)];

        return nullptr;
    }

    const ItemPrefixOrSuffixBase* ItemFactory::randomPrefixOrSuffixBase(const std::function<bool(const ItemPrefixOrSuffixBase&)>& filter) const
    {
        std::vector<const ItemPrefixOrSuffixBase*> pool;

        for (const auto& pair : mItemBaseHolder.getAllItemPrefixSuffixBases())
        {
            const ItemPrefixOrSuffixBase* base = pair.second.get();

            if (filter(*base))
            {
                for (int32_t i = 0; i < base->mDropRate; i++)
                    pool.push_back(base);
            }
        }

        if (!pool.empty())
            return pool[mRng.randomInRange(0, pool.size() - 1)];

        return nullptr;
    }

    void ItemFactory::applyRandomEnchantment(EquipmentItem& item, int32_t minLevel, int32_t maxLevel) const
    {
        bool prefix = mRng.randomInRange(0, 3) == 0;
        bool suffix = mRng.randomInRange(0, 2) != 0;

        if (!prefix && !suffix)
        {
            if (mRng.randomInRange(0, 1) == 1)
                suffix = true;
            else
                prefix = true;
        }

        if (prefix)
        {
            const ItemPrefixOrSuffixBase* prefixBase = randomPrefixOrSuffixBase([&](const ItemPrefixOrSuffixBase& base) {
                return base.mIsPrefix && base.canBeAppliedTo(item) && base.mQuality >= minLevel && base.mQuality <= maxLevel;
            });

            if (prefixBase)
            {
                item.mPrefix = prefixBase->create();
                item.mPrefix->init();
            }
        }

        if (suffix)
        {
            const ItemPrefixOrSuffixBase* suffixBase = randomPrefixOrSuffixBase([&](const ItemPrefixOrSuffixBase& base) {
                return !base.mIsPrefix && base.canBeAppliedTo(item) && base.mQuality >= minLevel && base.mQuality <= maxLevel;
            });

            if (suffixBase)
            {
                item.mSuffix = suffixBase->create();
                item.mSuffix->init();
            }
        }
    }

    void ItemFactory::saveItem(const Item& item, FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("Item", saver);

        saver.save(item.getBase()->mId);
        item.save(saver);
    }

    std::unique_ptr<Item> ItemFactory::loadItem(FASaveGame::GameLoader& loader) const
    {
        std::string baseId = loader.load<std::string>();
        std::unique_ptr<Item> newItem = mItemBaseHolder.createItem(baseId);
        newItem->load(loader);
        return newItem;
    }
}
