#include "potion.h"
#include "player.h"
#include <random/random.h>

namespace FAWorld
{
    // https:// wheybags.gitlab.io/jarulfs-guide/#potions-and-elixirs
    // Used for the formulas
    void Potion::restoreHp(Player& player)
    {
        FixedPoint bonus;
        switch (player.getClass())
        {
            case FAWorld::PlayerClass::warrior:
            {
                bonus = FixedPoint(2);
                break;
            }
            case FAWorld::PlayerClass::rogue:
            {
                bonus = FixedPoint("1.5");
                break;
            }
            case FAWorld::PlayerClass::sorceror:
            {
                bonus = FixedPoint(1);
                break;
            }
            default:
                break;
        }

        int32_t min = (int32_t)(bonus * FixedPoint(player.getStats().getHp().max) / FixedPoint(8)).floor();
        int32_t max = min * 3;
        int32_t toHeal = player.getWorld()->mRng->randomInRange(min, max);
        player.heal(toHeal);
    }

    void Potion::restoreHpFull(Player& player) { player.heal(); }

    void Potion::restoreMana(Player& player)
    {
        FixedPoint bonus;

        switch (player.getClass())
        {
            case FAWorld::PlayerClass::warrior:
            {
                bonus = FixedPoint(1);
                break;
            }
            case FAWorld::PlayerClass::rogue:
            {
                bonus = FixedPoint("1.5");
                break;
            }
            case FAWorld::PlayerClass::sorceror:
            {
                bonus = FixedPoint(2);
                break;
            }
            default:
                break;
        }

        int32_t min = (int32_t)(bonus * FixedPoint(player.getStats().getHp().max) / FixedPoint(8)).floor();
        int32_t max = min * 3;
        player.getWorld()->mRng->randomInRange(min, max);
        player.restoreMana();
    }

    void Potion::restoreManaFull(Player& player) { player.restoreMana(); }

    void Potion::increaseStrength(Player& player, int32_t delta) { player.addStrength(delta); }

    void Potion::increaseMagic(Player& player, int32_t delta) { player.addMagic(delta); }

    void Potion::increaseDexterity(Player& player, int32_t delta) { player.addDexterity(delta); }

    void Potion::increaseVitality(Player& player, int32_t delta) { player.addVitality(delta); }
}
