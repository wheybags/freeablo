
#pragma once

#include "../components/diabloexe/diabloexe.h"
#include <string>
#include "itemfactory.h"

namespace FAWorld
{

    class Player;
    class ActorStats;

    class PlayerFactory
    {
    public:
        PlayerFactory(const DiabloExe::DiabloExe& exe, const FAWorld::ItemFactory &itemFactory);

        Player* create(const std::string& playerClass) const;

    private:
        void loadTestingKit(Player* player) const;
        void createWarrior(Player* player) const;
        void createRogue(Player* player) const;
        void createSorcerer(Player* player) const;

        const DiabloExe::DiabloExe& mExe;
        const FAWorld::ItemFactory &mItemFactory;
    };
}
