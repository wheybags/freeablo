#pragma once

#include <vector>

namespace FAWorld
{
    class Item;
    class ItemFactory;

    /// class for storing and regenerating items sold in various stores
    class StoreData
    {
    public:
        explicit StoreData(const ItemFactory& itemFactory);

        void regenerateGriswoldBasicItems(int32_t ilvl);

    public:
        std::vector<Item> griswoldBasicItems;

    private:
        const ItemFactory& mItemFactory;
    };
}
