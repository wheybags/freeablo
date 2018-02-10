#include "itembonus.h"
#include "diabloexe/baseitem.h"

namespace FAWorld
{
    ItemBonus::ItemBonus(const DiabloExe::BaseItem& item) {
        minAttackDamage = item.minAttackDamage;
        maxAttackDamage = item.maxAttackDamage;
    }

    auto ItemBonus::operator+=(const ItemBonus::selfType& other) -> selfType&
    {
        minAttackDamage += other.minAttackDamage;
        maxAttackDamage += other.maxAttackDamage;
        return *this;
    }
}
