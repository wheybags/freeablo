#pragma once

#include <cstdint>

namespace DiabloExe
{
    class BaseItem;
}

namespace FAWorld
{
    /// ItemBonus is a class for storing addable properties of items which later applied to a player
    /// It was decided to not use enum indexed vector for it because some of stats should be summed as a bitmask
    /// In case of performance problem sum of ItemBonus'es can later be cached and updated only in case if equipment is changed
    class ItemBonus
    {
        using selfType = ItemBonus;

    public:
        explicit ItemBonus (const DiabloExe::BaseItem &item);
        ItemBonus () = default;

        int32_t minAttackDamage = 0;
        int32_t maxAttackDamage = 0;

        selfType& operator+=(const selfType& other);
        friend selfType operator+(selfType lhs, const selfType& rhs)
        {
            lhs += rhs;
            return lhs;
        }
    };
}
