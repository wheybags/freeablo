#include "simplebuffdebuffeffectbase.h"
#include "simplebuffdebuffeffect.h"
#include <diabloexe/exemagicitemeffect.h>
#include <misc/assert.h>

namespace FAWorld
{
    SimpleBuffDebuffEffectBase::SimpleBuffDebuffEffectBase(const DiabloExe::ExeMagicItemEffect& exeEffect) : MagicEffectBase(exeEffect)
    {
        switch (exeEffect.mEffect)
        {
            case ExeMagicEffectType::PlusStrength:
                mAttribute = Attribute::Strength;
                break;
            case ExeMagicEffectType::MinusStrength:
                mAttribute = Attribute::Strength;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusMagic:
                mAttribute = Attribute::Magic;
                break;
            case ExeMagicEffectType::MinusMagic:
                mAttribute = Attribute::Magic;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusDexterity:
                mAttribute = Attribute::Dexterity;
                break;
            case ExeMagicEffectType::MinusDexterity:
                mAttribute = Attribute::Dexterity;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusVitality:
                mAttribute = Attribute::Vitality;
                break;
            case ExeMagicEffectType::MinusVitality:
                mAttribute = Attribute::Vitality;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusToHit:
                mAttribute = Attribute::ToHit;
                break;
            case ExeMagicEffectType::MinusToHit:
                mAttribute = Attribute::ToHit;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusLife:
                mAttribute = Attribute::Life;
                break;
            case ExeMagicEffectType::MinusLife:
                mAttribute = Attribute::Life;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusMana:
                mAttribute = Attribute::Mana;
                break;
            case ExeMagicEffectType::MinusMana:
                mAttribute = Attribute::Mana;
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                break;
            case ExeMagicEffectType::PlusDamage:
                mAttribute = Attribute::Damage;
                break;
            case ExeMagicEffectType::MinusArmor:
                mAttribute = Attribute::Armor;
                break;
            default:
                invalid_enum(ExeMagicEffectType, exeEffect.mEffect);
        }
    }

    SimpleBuffDebuffEffectBase::~SimpleBuffDebuffEffectBase() = default;

    std::unique_ptr<MagicEffect> SimpleBuffDebuffEffectBase::create() const { return std::unique_ptr<MagicEffect>(new SimpleBuffDebuffEffect(this)); }
}
