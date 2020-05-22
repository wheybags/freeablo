#pragma once
#include "item.h"

namespace FAWorld
{
    class EquipmentItemBase;

    class EquipmentItem final : public Item2
    {
        using super = Item2;
    public:

        explicit EquipmentItem(const EquipmentItemBase* base);

        EquipmentItem* getAsEquipmentItem() override { return this; }

        const EquipmentItemBase* getBase() const;
    };
}
