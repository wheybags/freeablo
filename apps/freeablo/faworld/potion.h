#pragma once
#include "player.h"

namespace FAWorld
{
    // Class Potion for all potion functions
    class Potion
    {
    public:
        static void restoreHp(Player& player);
        static void restoreMana(Player& player);
        static void restoreHpFull(Player& player);
        static void restoreManaFull(Player& player);

        // elixirs
        static void increaseStrength(Player& player, int32_t delta);
        static void increaseMagic(Player& player, int32_t delta);
        static void increaseDexterity(Player& player, int32_t delta);
        static void increaseVitality(Player& player, int32_t delta);
    };
}
