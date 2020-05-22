#include "storedata.h"
#include "../fasavegame/gameloader.h"
#include "itemfactory.h"
#include <random/random.h>

namespace FAWorld
{
    StoreData::StoreData(const ItemFactory& itemFactory) : mItemFactory(itemFactory) {}

    void StoreData::regenerateGriswoldBasicItems(int32_t ilvl, Random::Rng& rng)
    {
        int32_t count = rng.randomInRange(10, 20);
        griswoldBasicItems.resize(count);
        for (auto& item : griswoldBasicItems)
        {
            ItemId itemId = mItemFactory.randomItemId(
                [ilvl](const DiabloExe::BaseItem& item) { return ItemFilter::maxQLvl(ilvl)(item) || ItemFilter::sellableGriswoldBasic()(item); });

            item.item = mItemFactory.generateBaseItem(itemId);
            item.storeId = mNextItemId;
            mNextItemId++;
        }

        std::sort(griswoldBasicItems.begin(), griswoldBasicItems.end(), [](const StoreItem& lhs, const StoreItem& rhs) {
            return lhs.item.baseId() < rhs.item.baseId();
        });
    }

    void StoreData::save(FASaveGame::GameSaver& saver) const
    {
        saver.save(uint32_t(griswoldBasicItems.size()));
        for (auto& item : griswoldBasicItems)
        {
            saver.save(item.storeId);
            item.item.save(saver);
        }

        saver.save(mNextItemId);
    }

    void StoreData::load(FASaveGame::GameLoader& loader)
    {
        griswoldBasicItems.clear();

        uint32_t size = loader.load<uint32_t>();
        griswoldBasicItems.resize(size);

        for (uint32_t i = 0; i < size; i++)
        {
            griswoldBasicItems[i].storeId = loader.load<uint32_t>();
            griswoldBasicItems[i].item.load(loader);
        }

        mNextItemId = loader.load<uint32_t>();
    }
}
