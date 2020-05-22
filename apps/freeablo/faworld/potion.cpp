#include "potion.h"
#include "item.h"
#include "player.h"
#include <random/random.h>

namespace FAWorld
{
    Potion::Potion(){};
    Potion::~Potion(){};

    void Potion::restoreHp(Player* player)
    {
        double bonus = 1.0;
        // bonus came from Jarulf's guide
        switch (player->getClass())
        {
            case FAWorld::PlayerClass::warrior: {
                bonus = 2.0;
                break;
            }
            case FAWorld::PlayerClass::rogue: {
                bonus = 1.5;
                break;
            }
            case FAWorld::PlayerClass::sorceror: {
                bonus = 1.0;
                break;
            }
            default:
                break;
        }

        // potion formula from jarulf's guide
        int32_t min = (bonus * player->getStats().getHp().max) / 8;
        int32_t max = min * 3;
        int32_t toHeal = player->getWorld()->mRng->randomInRange(min, max);
        player->heal(toHeal);
    };

    void Potion::restoreHpFull(Player* player) { player->heal(); }

    void Potion::restoreMana(Player* player)
    {
        double bonus = 1.0;
        // bonus came from Jarulf's guide
        switch (player->getClass())
        {
            case FAWorld::PlayerClass::warrior: {
                bonus = 1.0;
                break;
            }
            case FAWorld::PlayerClass::rogue: {
                bonus = 1.5;
                break;
            }
            case FAWorld::PlayerClass::sorceror: {
                bonus = 2.0;
                break;
            }
            default:
                break;
        }
        // potion formula from jarulf's guide
        int32_t min = (bonus * player->getStats().getHp().max) / 8;
        int32_t max = min * 3;
        player->getWorld()->mRng->randomInRange(min, max);
        player->restoreMana();
    };

    void Potion::restoreManaFull(Player* player) { player->restoreMana(); }
}
