#include "itemfactory.h"
#include "diabloexe/diabloexe.h"
#include "item.h"
#include "itemenums.h"
#include <engine/enginemain.h>
#include <fasavegame/gameloader.h>
#include <random/random.h>

namespace FAWorld
{
    ItemFilter::Callback ItemFilter::maxQLvl(int32_t value)
    {
        return [value](const DiabloExe::ExeItem& item) { return static_cast<int32_t>(item.qualityLevel) <= value; };
    }

    ItemFilter::Callback ItemFilter::sellableGriswoldBasic()
    {
        return [](const DiabloExe::ExeItem& item) {
            static const auto excludedTypes = {ItemType::misc, ItemType::gold, ItemType::staff, ItemType::ring, ItemType::amulet};
            return std::count(excludedTypes.begin(), excludedTypes.end(), static_cast<ItemType>(item.type)) == 0;
        };
    }

    ItemFactory::ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng) : mItemBaseHolder(exe), mExe(exe), mRng(rng)
    {
        for (int i = 0; i < static_cast<int>(mExe.getBaseItems().size()); ++i)
            mUniqueBaseItemIdToItemId[mExe.getBaseItems()[i].uniqueBaseItemId] = static_cast<ItemId>(i);
    }

    std::unique_ptr<Item2> ItemFactory::generateBaseItem(ItemId id, const BaseItemGenOptions& /*options*/) const
    {
        const std::string& lookup = Engine::EngineMain::get()->exe().getBaseItems()[int32_t(id)].idName;
        std::unique_ptr<Item2> newItem = mItemBaseHolder.createItem(lookup);
        newItem->init();

        return newItem;

        //        Item res;
        //        res.mEmpty = false;
        //        res.mIsReal = true;
        //        res.mInvX = 0;
        //        res.mInvY = 0;
        //        res.mBaseId = id;
        //        auto info = getInfo(id);
        //        res.mMaxDurability = res.mCurrentDurability = info.durability;
        //        res.mArmorClass = mRng.randomInRange(info.minArmorClass, info.maxArmorClass);
        //        return res;
    }

    ItemId ItemFactory::randomItemId(const ItemFilter::Callback& filter) const
    {
        static std::vector<ItemId> pool;
        pool.clear();
        for (auto id : enum_range<ItemId>())
        {
            const DiabloExe::ExeItem& info = getInfo(id);
            if (!filter(info))
                continue;
            for (int32_t i = 0; i < static_cast<int32_t>(info.dropRate); ++i)
                pool.push_back(id);
        }
        return pool[mRng.randomInRange(0, pool.size() - 1)];
    }

    const DiabloExe::ExeItem& ItemFactory::getInfo(ItemId id) const { return mExe.getBaseItems().at(static_cast<int>(id)); }

    void ItemFactory::saveItem(const Item2& item, FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("Item", saver);

        saver.save(item.getBase()->mId);
        item.save(saver);
    }

    std::unique_ptr<Item2> ItemFactory::loadItem(FASaveGame::GameLoader& loader) const
    {
        std::string baseId = loader.load<std::string>();
        std::unique_ptr<Item2> newItem = mItemBaseHolder.createItem(baseId);
        newItem->load(loader);
        return newItem;
    }
}
