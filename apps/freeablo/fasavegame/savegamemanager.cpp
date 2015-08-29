#include "savegamemanager.h"

#include "../faworld/world.h"
#include "../faworld/player.h"

#include <settings/settings.h>

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
    size_t level = mWorld->getCurrentLevelIndex();
    output << level;
    output << *player;
}

bool SaveGameManager::load()
{
    FAWorld::Player * player = mWorld->getPlayer();
    size_t tmpLevel;

    std::ifstream saveGameFile(getSaveGamePath());
    if(!saveGameFile.good())
    {
        return false;
    }

    InputStream input(saveGameFile);
    input >> tmpLevel;
    input >> *player;

    mWorld->setLevel(tmpLevel);
    return true;
}

std::string SaveGameManager::getSaveGamePath()
{
    // TO DO: read path from settings after PR #151 merge

    return "savegame.txt";
}

}
