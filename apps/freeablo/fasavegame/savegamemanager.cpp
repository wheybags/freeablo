#include "savegamemanager.h"

#include "../faworld/world.h"
#include "../faworld/player.h"

#include <settings/settings.h>

extern int32_t currentLevel;
extern int32_t changeLevel;


namespace FASaveGame
{

SaveGameManager::SaveGameManager(FAWorld::World * world):
    mWorld(world)
{}

void SaveGameManager::save()
{
    FAWorld::Player * player = mWorld->getPlayer();

    std::ofstream saveGameFile(getSaveGamePath());
    if(!saveGameFile.good())
    {
        return;
    }

    OutputStream output(saveGameFile);
    output << currentLevel;
    output << *player;
}

bool SaveGameManager::load()
{
    FAWorld::Player * player = mWorld->getPlayer();
    int tmpChangeLevel;

    std::ifstream saveGameFile(getSaveGamePath());
    if(!saveGameFile.good())
    {
        return false;
    }

    InputStream input(saveGameFile);
    input >> tmpChangeLevel;
    input >> *player;

    changeLevel = tmpChangeLevel - currentLevel;
    return true;
}

std::string SaveGameManager::getSaveGamePath()
{
    // TO DO: read path from settings after PR #151 merge

    return "savegame.txt";
}

}
