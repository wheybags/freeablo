#pragma once

namespace FAWorld
{
    class EquipmentItem;
    class MiscItem;

    class ItemBase;

    class Item2
    {
    protected:
        explicit Item2(const ItemBase* base) : mBase(base) {}

    public:
        virtual EquipmentItem* getAsEquipmentItem() { return nullptr; }
        const EquipmentItem* getAsEquipmentItem() const { return const_cast<Item2*>(this)->getAsEquipmentItem(); }

        virtual MiscItem* getAsMiscItem() { return nullptr; }
        const MiscItem* getAsMiscItem() const { return const_cast<Item2*>(this)->getAsMiscItem(); }

        const ItemBase* getBase() const { return mBase; }

    protected:
        const ItemBase* mBase = nullptr;
    };
}
