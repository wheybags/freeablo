#ifndef SAVEGAME_MANAGER_H
#define SAVEGAME_MANAGER_H

#include <string>
#include "savegame.h"

namespace FAWorld
{
    class World;
}

namespace FASaveGame
{

class SaveGameManager
{
public:
    SaveGameManager(FAWorld::World * world);

    void save();
    bool load();

private:

    std::string getSaveGamePath();

    FAWorld::World * mWorld;
};

}
#endif
