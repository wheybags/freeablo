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
    PlayerFactory(DiabloExe::DiabloExe & exe);

    Player* create(const std::string& playerClass);

private:

    void createWarrior();
    void createRogue();
    void createSorcerer();

    DiabloExe::DiabloExe & mExe;
    Player * mPlayer;
    DiabloExe::CharacterStats mCharStats;
    FAWorld::ActorStats * mStats;

};

}

#endif
