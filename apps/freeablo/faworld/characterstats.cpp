#include "characterstats.h"
#include "player.h"
#include "../falevelgen/random.h"
namespace FAWorld
{

    void CharacterStatsBase::processEffects()
    {
        std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> effects;
        effects = mPlayer->mInventory.getTotalEffects();
        printf("%lu -- size of effects\n", effects.size());
        for(std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>>::iterator it= effects.begin();it != effects.end(); ++it)
        {
            Item::ItemEffect effect;
            uint32_t minRange, maxRange, id;
            std::tie(effect, minRange, maxRange, id) = *it;
            uint32_t range = 0;
            range = FALevelGen::randomInRange(minRange, maxRange);
            switch(effect)
            {
                case Item::IncPercentChanceToHit:
                break;

                case Item::DecPercentChanceToHit:
                break;

                case Item::IncPercentDamageDone:
                break;

                case Item::DecPercentDamageDone:
                break;

                case Item::IncPercentDamageDoneChanceToHit:
                break;

                case Item::DecPercentDamageDoneChanceToHit:
                break;

                case Item::IncPercentArmourClass:
                break;

                case Item::DecPercentArmourClass:
                break;

                case Item::IncPercentResistFire:
                break;

                case Item::IncPercentResistLightning:
                break;

                case Item::IncPercentResistMagic:
                break;

                case Item::IncPercentResistAll:
                break;

                case Item::ModSpellLevel:
                break;

                case Item::IncCharges:
                break;

                case Item::IncFireDamage:
                break;

                case Item::IncLightningDamage:
                break;

                case Item::IncStrength:
                break;

                case Item::DecStrength:
                break;

                case Item::IncMagic:
                break;

                case Item::DecMagic:
                break;

                case Item::IncDexterity:
                break;

                case Item::DecDexterity:
                break;

                case Item::IncVitality:
                break;

                case Item::DecVitality:
                break;

                case Item::IncAllBasicStats:
                break;

                case Item::DecAllBasicStats:
                break;

                case Item::IncDamageTaken:                    
                break;

                case Item::DecDamageTaken:                    
                break;

                case Item::IncHP:                    
                break;

                case Item::DecHP:                    
                break;

                case Item::IncMana:                    
                break;

                case Item::DecMana:                    
                break;

                case Item::IncPercentDurability:
                break;

                case Item::DecPercentDurability:
                break;

                case Item::Indestructible:
                break;

                case Item::IncLightRadius:
                break;

                case Item::DecLightRadius:
                break;

                case Item::Unknown0:
                break;

                case Item::MultipleArrows:
                break;

                case Item::IncPercentFireArrowDamage:
                break;

                case Item::IncPercentLightningArrowDamage:
                break;

                case Item::UniquePicture:
                break;

                case Item::Thorns:
                break;

                case Item::AllMana:
                break;

                case Item::PlayerNoHeal:
                break;

                case Item::HalfTrapDamage:
                break;

                case Item::Knockback:
                break;

                case Item::MonsterNoHeal:
                break;

                case Item::PercentManaSteal:
                break;

                case Item::PercentLifeSteal:
                break;

                case Item::ArmourPenetration:
                break;

                case Item::AttackSpeed0:
                break;

                case Item::HitRecovery:
                break;

                case Item::FastBlock:
                break;

                case Item::IncDamageDone:
                break;

                case Item::RandomArrowSpeed:
                break;

                case Item::UnusualDamage:
                break;

                case Item::AlteredDurability:
                break;

                case Item::NoStrengthRequirment:
                break;

                case Item::Spell:
                break;

                case Item::AttackSpeed1:
                break;

                case Item::OneHanded:
                break;

                case Item::AntiDemon:
                break;

                case Item::ZeroAllResist:
                break;

                case Item::ConstantLifeDrain:
                break;

                case Item::PercentFixedLifeSteal:
                break;

                case Item::Infravision:
                break;

                case Item::SpecifiedArmourClass:
                break;

                case Item::IncHPWithArmourClass:
                break;

                case Item::IncArmourClassWithMana:
                break;

                case Item::IncFireResistWithLevel:
                break;

                case Item::DecArmourClass:
                break;

                default:
                break;

            }
        }
    }

    uint32_t CharacterStatsBase::getDamage()
    {
        std::cout << __LINE__ << " here" << std::endl;
        return 0;

    }

    double CharacterStatsBase::getChanceToHitMelee()
    {
        return 0;
    }

    void CharacterStatsBase::takeDamage(double amount)
    {

    }

    void MeleeStats::recalculateDerivedStats()
    {
        clearDerivedStats();
        processEffects();
    }

    void RangerStats::recalculateDerivedStats()
    {
        clearDerivedStats();
        processEffects();

    }

    void MageStats::recalculateDerivedStats()
    {
        clearDerivedStats();
        processEffects();


    }

}
