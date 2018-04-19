#pragma once
#include <cstdint>
#include <vector>

namespace Random
{
    class Rng;
}

namespace FAWorld
{
    class Item;
    class ItemFactory;

    /// class for storing and regenerating items sold in various stores
    class StoreData
    {
    public:
        explicit StoreData(const ItemFactory& itemFactory);

        void regenerateGriswoldBasicItems(int32_t ilvl, Random::Rng& rng);

    public:
        std::vector<Item> griswoldBasicItems;

    private:
        const ItemFactory& mItemFactory;
    };
}
