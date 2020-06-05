#include "itemprefixorsuffixbase.h"
#include "itemprefixorsuffix.h"
#include <diabloexe/exemagicitemeffect.h>
#include <faworld/magiceffects/simplebuffdebuffeffectbase.h>

namespace FAWorld
{
    ItemPrefixOrSuffixBase::ItemPrefixOrSuffixBase(const DiabloExe::ExeMagicItemEffect& exeEffect)
        : mName(exeEffect.mName), mCursed(!exeEffect.mNotCursed), mIsPrefix(exeEffect.mIsPrefix)
    {
        switch (exeEffect.mEffect)
        {
            case ExeMagicEffectType::PlusStrength:
            case ExeMagicEffectType::MinusStrength:
            case ExeMagicEffectType::PlusMagic:
            case ExeMagicEffectType::MinusMagic:
            case ExeMagicEffectType::PlusDexterity:
            case ExeMagicEffectType::MinusDexterity:
            case ExeMagicEffectType::PlusVitality:
            case ExeMagicEffectType::MinusVitality:
            case ExeMagicEffectType::PlusToHit:
            case ExeMagicEffectType::MinusToHit:
            case ExeMagicEffectType::PlusLife:
            case ExeMagicEffectType::MinusLife:
            case ExeMagicEffectType::PlusMana:
            case ExeMagicEffectType::MinusMana:
            case ExeMagicEffectType::PlusDamage:
            case ExeMagicEffectType::MinusArmor:
                mEffects.emplace_back(new SimpleBuffDebuffEffectBase(exeEffect));
                break;

            case ExeMagicEffectType::PlusDamagePercent:
            case ExeMagicEffectType::MinusDamagePercent:
            case ExeMagicEffectType::PlusToHitAndDamagePercent:
            case ExeMagicEffectType::MinusToHitAndDamagePercent:
            case ExeMagicEffectType::PlusArmorPercent:
            case ExeMagicEffectType::MinusArmorPercent:
            case ExeMagicEffectType::PlusResistFire:
            case ExeMagicEffectType::PlusResistLightning:
            case ExeMagicEffectType::PlusResistMagic:
            case ExeMagicEffectType::PlusResistAll:
            case ExeMagicEffectType::PlusSpellLevels:
            case ExeMagicEffectType::PlusCharges:
            case ExeMagicEffectType::PlusDamageFire:
            case ExeMagicEffectType::PlusDamageLightning:
            case ExeMagicEffectType::PlusAllAttributes:
            case ExeMagicEffectType::MinusAllAttributes:
            case ExeMagicEffectType::PlusDamageTaken:
            case ExeMagicEffectType::MinusDamageTaken:
            case ExeMagicEffectType::PlusDurabilityPercent:
            case ExeMagicEffectType::CurseDurabilityPercent:
            case ExeMagicEffectType::Indestructible:
            case ExeMagicEffectType::PlusLightPercent:
            case ExeMagicEffectType::CurseLightPercent:
            case ExeMagicEffectType::MultipleArrows:
            case ExeMagicEffectType::PlusFireArrowDamage:
            case ExeMagicEffectType::PlusLightningArrowDamage:
            case ExeMagicEffectType::UniqueIcon:
            case ExeMagicEffectType::Deal1To3DamageToAttackers:
            case ExeMagicEffectType::ZeroMana:
            case ExeMagicEffectType::UserCantHeal:
            case ExeMagicEffectType::AbsorbHalfTrapDamage:
            case ExeMagicEffectType::Knockback:
            case ExeMagicEffectType::HitMonsterCantHeal:
            case ExeMagicEffectType::StealMana:
            case ExeMagicEffectType::StealLife:
            case ExeMagicEffectType::DamageArmor:
            case ExeMagicEffectType::FastAttack:
            case ExeMagicEffectType::FastHitRecovery:
            case ExeMagicEffectType::FastBlock:
            case ExeMagicEffectType::RandomSpeedArrows:
            case ExeMagicEffectType::SetItemDamage:
            case ExeMagicEffectType::SetDurability:
            case ExeMagicEffectType::NoStrengthRequirement:
            case ExeMagicEffectType::SetCharges:
            case ExeMagicEffectType::FastAttack2:
            case ExeMagicEffectType::OneHanded:
            case ExeMagicEffectType::Plus200PercentOnDemons:
            case ExeMagicEffectType::AllResistancesZero:
            case ExeMagicEffectType::ConstantlyLoseLife:
            case ExeMagicEffectType::LifeSteal:
            case ExeMagicEffectType::Infravision:
            case ExeMagicEffectType::SetArmor:
            case ExeMagicEffectType::AddArmorToLife:
            case ExeMagicEffectType::Add10PercentOfManaToArmor:
            case ExeMagicEffectType::PlusLevelDependentResistFire:
                mEffects.emplace_back(new MagicEffectBase(exeEffect)); // Not yet handled, use default
                break;
        }
    }

    ItemPrefixOrSuffixBase::~ItemPrefixOrSuffixBase() = default;

    std::unique_ptr<ItemPrefixOrSuffix> ItemPrefixOrSuffixBase::create() const { return std::make_unique<ItemPrefixOrSuffix>(this); }
}