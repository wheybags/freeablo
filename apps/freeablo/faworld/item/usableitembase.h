#pragma once
#include "itembase.h"
#include <functional>

namespace FAWorld
{
    class Player;

    class UsableItemBase final : public ItemBase
    {
        using super = ItemBase;

    public:
        explicit UsableItemBase(const DiabloExe::ExeItem& exeItem);
        std::unique_ptr<Item> createItem() const override;
        bool isBeltEquippable() const;

    public:
        std::function<void(Player&)> mEffect;
        std::string mUseSoundPath;
    };
}
