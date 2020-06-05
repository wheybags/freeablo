#include "simplebuffdebuffeffect.h"
#include "simplebuffdebuffeffectbase.h"
#include <faworld/actorstats.h>
#include <misc/misc.h>

namespace FAWorld
{
    SimpleBuffDebuffEffect::SimpleBuffDebuffEffect(const SimpleBuffDebuffEffectBase* base) : super(base) {}

    void SimpleBuffDebuffEffect::init()
    {
        mValue = getBase()->mParameter2; // TODO: this should be randomised
    }

    void SimpleBuffDebuffEffect::apply(MagicStatModifiers& modifiers)
    {
        switch (getBase()->mAttribute)
        {
            case SimpleBuffDebuffEffectBase::Attribute::Strength:
                modifiers.baseStats.strength += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Magic:
                modifiers.baseStats.magic += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Dexterity:
                modifiers.baseStats.dexterity += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Vitality:
                modifiers.baseStats.vitality += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::ToHit:
                modifiers.toHit += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Life:
                modifiers.maxLife += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Mana:
                modifiers.maxMana += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Damage:
                modifiers.meleeDamageBonus += mValue;
                modifiers.rangedDamageBonus += mValue;
                break;
            case SimpleBuffDebuffEffectBase::Attribute::Armor:
                modifiers.armorClass += mValue;
                break;
        }
    }

    const SimpleBuffDebuffEffectBase* SimpleBuffDebuffEffect::getBase() const { return safe_downcast<const SimpleBuffDebuffEffectBase*>(mBase); }
}