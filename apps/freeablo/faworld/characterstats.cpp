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
    void CharacterStatsBase::takeDamage(double amount)
    {

    }

    double CharacterStatsBase::getArmourClass()
    {
        return mArmourClass;
    }

    Item CharacterStatsBase::getCurrentWeapon()
    {
        Inventory& inv = mPlayer->mInventory;

        Item item;

        if(inv.getItemAt(Item::eqLEFTHAND).isEmpty() &&
           inv.getItemAt(Item::eqRIGHTHAND).isEmpty())
            item=Item::empty;

        //One hand empty
        else if((!inv.getItemAt(Item::eqLEFTHAND).isEmpty() &&
                inv.getItemAt(Item::eqRIGHTHAND).isEmpty()) ||
                (inv.getItemAt(Item::eqLEFTHAND).isEmpty() &&
                !inv.getItemAt(Item::eqRIGHTHAND).isEmpty()))
        {
            Item hand;
            if(inv.getItemAt(Item::eqLEFTHAND).isEmpty())
                hand = inv.getItemAt(Item::eqLEFTHAND);
            else
                hand = inv.getItemAt(Item::eqRIGHTHAND);
            item = hand;
        }

        else if(!inv.getItemAt(Item::eqLEFTHAND).isEmpty() &&
                !inv.getItemAt(Item::eqRIGHTHAND).isEmpty())
        {
            //Duel wielding with one item (Cleaver/Axe)
            if(inv.getItemAt(Item::eqLEFTHAND) == inv.getItemAt(Item::eqRIGHTHAND))
            {
                item = inv.getItemAt(Item::eqLEFTHAND);
            }

            //Duel wielding shield and other, take damage value from sword
            else
            {
                if(!(inv.getItemAt(Item::eqLEFTHAND).getCode() == Item::icShield))
                    item = inv.getItemAt(Item::eqLEFTHAND);
                else
                    item = inv.getItemAt(Item::eqRIGHTHAND);

            }
        }
        return item;
    }

    double CharacterStatsBase::getItemDamage()
    {
        Item item = getCurrentWeapon();
        if(!item.isEmpty())
            return getCurrentWeapon().getAttackDamage();
        else
            return 1;
    }

    double CharacterStatsBase::getMonsterBonusDamage(MonsterStats *enemy)
    {
        mBonusAgainstDemons=false;
        mBonusAgainstAnimal=false;
        mBonusAgainstUndead=false;
        Item item = getCurrentWeapon();

        if(item.isAntiDemon() && enemy->mMonster->getType() == Monster::demon)
            return 3;
        else if(item.getCode() == Item::icBlunt && enemy->mMonster->getType() == Monster::undead)
            return 2;
        else if(item.getCode() == Item::icSword && enemy->mMonster->getType() == Monster::animal)
            return 1.5;
        else
            return 1;
    }

    double MeleeStats::getChanceToHitMelee(MonsterStats * enemy)
    {
        double chance = (CHANCE_TO_HIT_BASE + (mDexterity + mBonusDexterity)/2
                         + mLevel + WARRIOR_MELEE_HIT_CHANCE_BONUS) - enemy->getArmourClass();
        if (chance < 5)
            chance=5;
        else if(chance > 95)
            chance = 95;
        return chance;
    }

    double MeleeStats::getMeleeDamage(MonsterStats *enemy)
    {
        double charDamage = (mStrength * mLevel)/100;
        double itemDamage = getItemDamage();
        double finalDamage = charDamage + itemDamage;
        finalDamage *= getMonsterBonusDamage(enemy);
        if(FALevelGen::percentageChance(mLevel) && mPlayer->getClassName()=="warrior")
            finalDamage*=2;
        printf("finalDamage: %f\n", finalDamage);
        return finalDamage;
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
