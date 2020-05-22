#pragma once
#include "item.h"

namespace FAWorld
{
    class MiscItemBase;

    class MiscItem final : public Item2
    {
        using super = Item2;
    public:

        explicit MiscItem(const MiscItemBase* base);

        MiscItem* getAsMiscItem() override { return this; }

        const MiscItemBase* getBase() const;
    };
}
