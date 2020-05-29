#pragma once
#include "itembase.h"

namespace FAWorld
{
    class GoldItemBase final : public ItemBase
    {
        using super = ItemBase;

    public:
        explicit GoldItemBase(const DiabloExe::ExeItem& exeItem);

        std::unique_ptr<Item> createItem() const override;

    public:
        int32_t mMaxCount = 5000;
    };
}
