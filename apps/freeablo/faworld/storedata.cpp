#include "storedata.h"
#include "item.h"
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
            item = mItemFactory.generateBaseItem(mItemFactory.randomItemId(ItemFilter::maxQLvl(ilvl), ItemFilter::sellableGriswoldBasic()));
        std::sort(griswoldBasicItems.begin(), griswoldBasicItems.end(), [](const Item& lhs, const Item& rhs) { return lhs.baseId() < rhs.baseId(); });
    }
}
