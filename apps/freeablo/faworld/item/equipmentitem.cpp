#include "equipmentitem.h"
#include "equipmentitembase.h"
#include "itemprefixorsuffix.h"
#include "itemprefixorsuffixbase.h"
#include <fasavegame/gameloader.h>
#include <fmt/format.h>
#include <misc/misc.h>

namespace FAWorld
{
    EquipmentItem::EquipmentItem(const EquipmentItemBase* base) : super(base) {}

    void EquipmentItem::init()
    {
        // TODO: this should be randomised in range
        mArmorClass = getBase()->mArmorClassRange.max;
    }

    void EquipmentItem::save(FASaveGame::GameSaver& saver) const { saver.save(mArmorClass); }

    void EquipmentItem::load(FASaveGame::GameLoader& loader)
    {
        mArmorClass = Misc::clamp(loader.load<int32_t>(), getBase()->mArmorClassRange.min, getBase()->mArmorClassRange.max);
    }

    const EquipmentItemBase* EquipmentItem::getBase() const { return safe_downcast<const EquipmentItemBase*>(mBase); }

    std::string EquipmentItem::getFullDescription() const
    {
        std::string description;
        if (mPrefix)
            description += mPrefix->getBase()->mName + " ";

        description += getBase()->mName;

        if (mSuffix)
            description += " of " + mSuffix->getBase()->mName;

        description += "\n";

        if (getBase()->mClass == ItemClass::weapon)
            description += fmt::format("damage: {} - {}\n", getBase()->mDamageBonusRange.start, getBase()->mDamageBonusRange.end);
        else
            description += fmt::format("armor: {}\n", mArmorClass);

        if (mPrefix)
            description += mPrefix->getFullDescription();
        if (mSuffix)
            description += mSuffix->getFullDescription();

        // TODO: durability
        // TODO: charges
        // TODO: requirements

        return description;
    }

    bool EquipmentItem::isMeleeWeapon() const { return getBase()->mClass == ItemClass::weapon && getBase()->mType != ItemType::bow; }
    bool EquipmentItem::isRangedWeapon() const { return getBase()->mType == ItemType::bow; }
}
