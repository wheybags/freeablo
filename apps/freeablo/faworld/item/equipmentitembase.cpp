#include "equipmentitembase.h"
#include <diabloexe/baseitem.h>
#include <faworld/item/equipmentitem.h>
#include <faworld/player.h>

namespace FAWorld
{
    EquipmentItemBase::EquipmentItemBase(const DiabloExe::ExeItem& exeItem)
        : super(exeItem), mDamageBonusRange(exeItem.minAttackDamage, exeItem.maxAttackDamage), mArmorClassRange(exeItem.minArmorClass, exeItem.maxArmorClass),
          mEquipSlot(exeItem.equipType), mRequiredStrength(exeItem.requiredStrength), mRequiredDexterity(exeItem.requiredDexterity),
          mRequiredMagic(exeItem.requiredMagic)
    {
    }

    bool EquipmentItemBase::usableByPlayer(const Player& player) const
    {
        const BaseStats& stats = player.getStats().getCalculatedStats().baseStats;
        return stats.strength >= mRequiredStrength && stats.dexterity >= mRequiredDexterity && stats.magic >= mRequiredMagic;
    }

    std::unique_ptr<Item> EquipmentItemBase::createItem() const { return std::unique_ptr<Item>(new EquipmentItem(this)); }
}