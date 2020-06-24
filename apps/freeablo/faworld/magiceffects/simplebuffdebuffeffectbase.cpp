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
            case ExeMagicEffectType::MinusStrength:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusStrength:
                mAttribute = Attribute::Strength;
                break;

            case ExeMagicEffectType::MinusMagic:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusMagic:
                mAttribute = Attribute::Magic;
                break;

            case ExeMagicEffectType::MinusDexterity:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusDexterity:
                mAttribute = Attribute::Dexterity;
                break;

            case ExeMagicEffectType::MinusVitality:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusVitality:
                mAttribute = Attribute::Vitality;
                break;

            case ExeMagicEffectType::MinusToHit:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusToHit:
                mAttribute = Attribute::ToHit;
                break;

            case ExeMagicEffectType::MinusLife:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusLife:
                mAttribute = Attribute::Life;
                break;

            case ExeMagicEffectType::MinusMana:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                [[fallthrough]];
            case ExeMagicEffectType::PlusMana:
                mAttribute = Attribute::Mana;
                break;

            case ExeMagicEffectType::PlusDamage:
                mAttribute = Attribute::Damage;
                break;

            case ExeMagicEffectType::MinusArmor:
                mParameter1 = -mParameter1;
                mParameter2 = -mParameter2;
                mAttribute = Attribute::Armor;
                break;
            default:
                invalid_enum(ExeMagicEffectType, exeEffect.mEffect);
        }

        switch (mAttribute)
        {
            case Attribute::Strength:
                mDescriptionFormatString = "{:+} to strength";
                break;
            case Attribute::Magic:
                mDescriptionFormatString = "{:+} to magic";
                break;
            case Attribute::Dexterity:
                mDescriptionFormatString = "{:+} to dexterity";
                break;
            case Attribute::Vitality:
                mDescriptionFormatString = "{:+} to vitality";
                break;
            case Attribute::ToHit:
                mDescriptionFormatString = "chance to hit : {:+}%";
                break;
            case Attribute::Life:
                mDescriptionFormatString = "Hit Points : {:+}";
                break;
            case Attribute::Mana:
                mDescriptionFormatString = "Mana : {:+}";
                break;
            case Attribute::Damage:
                mDescriptionFormatString = "{:+} points to damage";
                break;
            case Attribute::Armor:
                mDescriptionFormatString = "armor class: {:+}";
                break;
        }
    }

    SimpleBuffDebuffEffectBase::~SimpleBuffDebuffEffectBase() = default;

    std::unique_ptr<MagicEffect> SimpleBuffDebuffEffectBase::create() const { return std::unique_ptr<MagicEffect>(new SimpleBuffDebuffEffect(this)); }
}
