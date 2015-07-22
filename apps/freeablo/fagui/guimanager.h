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
    void placeItem(uint32_t to,
                   uint32_t from,
                   uint32_t fromY=0,
                   uint32_t fromX=0,
                   uint32_t toY=0,
                   uint32_t toX=0);
}

#endif
