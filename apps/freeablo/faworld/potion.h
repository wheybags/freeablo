#pragma once
#include "item.h"
#include "player.h"

namespace FAWorld
{
    // Class Potion for all potion functions
    static class Potion
    {
    public:
        void restoreHp(Player* player);
        void restoreMana(Player* player);
        void restoreHpFull(Player* player);
        void restoreManaFull(Player* player);

        //exliers
        void increaseStrength(Player* player, int32_t delta);
        void increaseMagic(Player* player, int32_t delta);
        void increaseDexterity(Player* player, int32_t delta);
        void increaseVitality(Player* player, int32_t delta);
        
    };
}
