#pragma once
#include "../components/diabloexe/diabloexe.h"
#include "enums.h"
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

        Player* create(World& world, PlayerClass playerClass) const;

    private:
        void loadTestingKit(Player* player) const;
        void fillWithGold(Player* player) const;
        void fillWithPotions(Player* player) const;
        void addWarriorItems(Player* player) const;
        void addRogueItems(Player* player) const;
        void addSorcerorItems(Player* player) const;

        const DiabloExe::DiabloExe& mExe;
        const FAWorld::ItemFactory& mItemFactory;
    };
}
