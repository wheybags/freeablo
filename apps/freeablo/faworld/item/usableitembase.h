#pragma once
#include "itembase.h"

namespace FAWorld
{
    class UsableItemBase final : public ItemBase
    {
        using super = ItemBase;

    public:
        explicit UsableItemBase(const DiabloExe::ExeItem& exeItem);

        std::unique_ptr<Item2> createItem() const override;

        bool isBeltEquippable() const;

    public:
    };
}
