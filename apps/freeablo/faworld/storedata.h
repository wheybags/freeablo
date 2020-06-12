#pragma once
#include <cstdint>
#include <faworld/item/item.h>
#include <vector>

namespace Random
{
    class Rng;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class ItemFactory;

    struct StoreItem
    {
        std::unique_ptr<Item> item;
        uint32_t storeId = 0;
    };

    /// class for storing items sold in various stores
    class StoreData
    {
    public:
        explicit StoreData(const ItemFactory& itemFactory);

        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);

        void generateGriswoldBasicItems(int32_t itemLevel, Random::Rng& rng);

    public:
        std::vector<StoreItem> griswoldBasicItems;

    private:
        uint32_t mNextItemId = 0;
        const ItemFactory& mItemFactory;
    };
}
