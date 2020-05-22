#pragma once
#include "item.h"
#include "player.h"

namespace FAWorld
{
    // Class Potion for all potion functions
    class Potion
    {
    public:
        Potion();
        ~Potion();

        void restoreHp(Player* player);
        void restoreMana(Player* player);
        void restoreHpFull(Player* player);
        void restoreManaFull(Player* player);

        /* for later elixer implimentation
        void increaseStr(Player* player);
        void increaseMagic(Player* player);
        void increaseDex(Player* player);
        void increaseVit(Player* player);
        */
    };
}
