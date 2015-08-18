#include <string>
#include "../faworld/itemmanager.h"
#ifndef GUIMANAGER_H
#define GUIMANAGER_H

namespace FAGui
{
    void initGui(FAWorld::Inventory &playerInventory, std::string invclass);
    void showIngameGui();
    void showMainMenu();
    void updateGui();
    void placeItem(uint32_t toPara,
                   uint32_t fromPara,
                   uint32_t fromY=0,
                   uint32_t fromX=0,
                   uint32_t toY=0,
                   uint32_t toX=0,
                   uint32_t beltX=0);
    extern std::string cursorPath;
    static std::string invClass;
    extern uint32_t cursorFrame;
}

#endif
