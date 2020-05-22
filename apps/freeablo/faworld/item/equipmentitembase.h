#pragma once
#include "itembase.h"

namespace FAWorld
{
    class EquipmentItemBase final : public ItemBase
    {
        using super = ItemBase;
    public:
        explicit EquipmentItemBase(const DiabloExe::ExeItem& exeItem);

    public:
        IntRange mDamageBonusRange;
        ItemEquipType mEquipSlot = ItemEquipType::none;
    };
}
