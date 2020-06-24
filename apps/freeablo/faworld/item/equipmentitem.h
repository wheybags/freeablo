#pragma once
#include "item.h"

namespace FAWorld
{
    class EquipmentItemBase;
    class ItemPrefixOrSuffix;

    class EquipmentItem final : public Item
    {
        using super = Item;

    public:
        explicit EquipmentItem(const EquipmentItemBase* base);

        void init() override;

        void save(FASaveGame::GameSaver& saver) const override;
        void load(FASaveGame::GameLoader& loader) override;

        EquipmentItem* getAsEquipmentItem() override { return this; }
        const EquipmentItemBase* getBase() const;

        bool isMeleeWeapon() const;
        bool isRangedWeapon() const;

        std::string getFullDescription() const override;

    public:
        int32_t mArmorClass = 0;
        std::unique_ptr<ItemPrefixOrSuffix> mPrefix;
        std::unique_ptr<ItemPrefixOrSuffix> mSuffix;
    };
}
