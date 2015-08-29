#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <string>

#include <level/itemmanager.h>

#include "fapython.h"

namespace FAGui
{
    class GuiManager
    {
        public:
        GuiManager(FAWorld::Inventory &playerInventory, Engine::EngineMain& engine);
        void showIngameGui();
        void showMainMenu();
        void updateGui();

        FAPythonFuncs mPythonFuncs;
    };

    extern std::string cursorPath;
    extern uint32_t cursorFrame;
}

#endif
