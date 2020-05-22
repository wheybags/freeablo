#include "equipmentitembase.h"

namespace FAWorld
{
    EquipmentItemBase::EquipmentItemBase(const DiabloExe::ExeItem& exeItem)
        : super(exeItem), mDamageBonusRange(exeItem.minAttackDamage, exeItem.maxAttackDamage), mEquipSlot(exeItem.equipType)
    {
    }
}