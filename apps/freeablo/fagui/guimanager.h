#include <string>
#include <level/itemmanager.h>
#ifndef GUIMANAGER_H
#define GUIMANAGER_H

namespace FAGui
{
    void initGui(FAWorld::Inventory &playerInventory);
    void showIngameGui();
    void showMainMenu();
    void updateGui();
}

#endif
