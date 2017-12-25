
#pragma once

#include "../components/diabloexe/diabloexe.h"
#include <string>

namespace FAWorld
{

    class Player;
    class ActorStats;

    class PlayerFactory
    {
    public:
        PlayerFactory(const DiabloExe::DiabloExe& exe);

        Player* create(const std::string& playerClass) const;

    private:
        static void loadTestingKit(Player* player);
        void createWarrior(Player* player) const;
        void createRogue(Player* player) const;
        void createSorcerer(Player* player) const;

        const DiabloExe::DiabloExe& mExe;
    };
}

