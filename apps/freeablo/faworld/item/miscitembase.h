#pragma once
#include "itembase.h"

namespace FAWorld
{
    class MiscItemBase final : public ItemBase
    {
        using super = ItemBase;

    public:
        explicit MiscItemBase(const DiabloExe::ExeItem& exeItem);

    public:
    };
}
