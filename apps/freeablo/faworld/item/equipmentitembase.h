#pragma once
#include "itembase.h"

namespace FAWorld
{
    class Player;

    class EquipmentItemBase final : public ItemBase
    {
        using super = ItemBase;

    public:
        explicit EquipmentItemBase(const DiabloExe::ExeItem& exeItem);

        std::unique_ptr<Item> createItem() const override;

        ItemEquipType getEquipType() const override { return mEquipSlot; }
        bool usableByPlayer(const Player& player) const;

    public:
        IntRange mDamageBonusRange;
        IntRange mArmorClassRange;
        ItemEquipType mEquipSlot = ItemEquipType::none;

        int32_t mRequiredStrength = 0;
        int32_t mRequiredDexterity = 0;
        int32_t mRequiredMagic = 0;
    };
}
