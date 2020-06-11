#include "storedata.h"
#include "../fasavegame/gameloader.h"
#include "itemfactory.h"
#include <engine/enginemain.h>
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
                [ilvl](const DiabloExe::ExeItem& item) { return ItemFilter::maxQLvl(ilvl)(item) || ItemFilter::sellableGriswoldBasic()(item); });

            item.item = mItemFactory.generateBaseItem(itemId);
            item.storeId = mNextItemId;
            mNextItemId++;
        }

        std::sort(griswoldBasicItems.begin(), griswoldBasicItems.end(), [](const StoreItem& lhs, const StoreItem& rhs) {
            return lhs.item->getBase()->mId < rhs.item->getBase()->mId;
        });
    }

    void StoreData::save(FASaveGame::GameSaver& saver) const
    {
        saver.save(uint32_t(griswoldBasicItems.size()));
        for (auto& item : griswoldBasicItems)
        {
            saver.save(item.storeId);
            Engine::EngineMain::get()->mWorld->getItemFactory().saveItem(*item.item, saver);
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
            griswoldBasicItems[i].item = Engine::EngineMain::get()->mWorld->getItemFactory().loadItem(loader);
        }

        mNextItemId = loader.load<uint32_t>();
    }
}
