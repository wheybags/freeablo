#include "storedata.h"
#include "item.h"
#include "itemfactory.h"
#include "misc/random.h"

namespace FAWorld
{
    StoreData::StoreData(const ItemFactory& itemFactory) : mItemFactory(itemFactory) {}

    void StoreData::regenerateGriswoldBasicItems(int32_t ilvl)
    {
        int32_t count = Random::randomInRange(10, 20);
        griswoldBasicItems.resize(count);
        for (auto& item : griswoldBasicItems)
            item = mItemFactory.generateBaseItem(mItemFactory.randomItemId(ItemFilter::maxQLvl(ilvl), ItemFilter::sellableGriswoldBasic()));
        std::sort(griswoldBasicItems.begin(), griswoldBasicItems.end(), [](const Item& lhs, const Item& rhs) { return lhs.baseId() < rhs.baseId(); });
    }
}
