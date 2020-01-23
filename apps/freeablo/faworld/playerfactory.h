
#pragma once

#include "../components/diabloexe/diabloexe.h"
#include "itemfactory.h"
#include <string>

namespace FAWorld
{
    class World;
    class Player;
    class ActorStats;

    class PlayerFactory
    {
    public:
        PlayerFactory(const DiabloExe::DiabloExe& exe, const FAWorld::ItemFactory& itemFactory);

        Player* create(World& world, const std::string& playerClass) const;

    private:
        void loadTestingKit(Player* player) const;
        void fillWithGold(Player* player) const;
        void fillWithPotions(Player* player) const;
        void createWarrior(Player* player) const;
        void createRogue(Player* player) const;
        void createSorcerer(Player* player) const;

        const DiabloExe::DiabloExe& mExe;
        const FAWorld::ItemFactory& mItemFactory;
    };
}
