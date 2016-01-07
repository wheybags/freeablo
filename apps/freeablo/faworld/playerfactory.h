#ifndef PLAYERFACTORY_H
#define PLAYERFACTORY_H

#include <string>
#include "../components/diabloexe/diabloexe.h"

namespace FAWorld
{

class Player;
class ActorStats;

class PlayerFactory
{
public:
    PlayerFactory(const DiabloExe::DiabloExe & exe);

    Player* create(const std::string& playerClass) const;

private:

    ActorStats* createWarrior(Player* player, const DiabloExe::CharacterStats& charStats) const;
    ActorStats* createRogue(Player* player, const DiabloExe::CharacterStats& charStats) const;
    ActorStats* createSorcerer(Player* player, const DiabloExe::CharacterStats& charStats) const;

    const DiabloExe::DiabloExe& mExe;
};

}

#endif
