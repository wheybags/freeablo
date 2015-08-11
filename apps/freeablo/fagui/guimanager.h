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
    void placeItem(uint32_t toPara,
                   uint32_t fromPara,
                   uint32_t fromY=0,
                   uint32_t fromX=0,
                   uint32_t toY=0,
                   uint32_t toX=0);
    extern std::string cursorPath;
    extern uint32_t cursorFrame;
}

#endif
