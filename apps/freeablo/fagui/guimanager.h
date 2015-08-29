#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "../faworld/itemmanager.h"
#include <string>


#include "fapython.h"

namespace FAGui
{

    class GuiManager
    {
        public:
        GuiManager(FAWorld::Inventory &playerInventory, Engine::EngineMain& engine, std::string invClass);
        void showIngameGui();
        void showMainMenu();
        void updateGui();
        static std::string invClass;

        FAPythonFuncs mPythonFuncs;
    };

    extern std::string cursorPath;

    extern uint32_t cursorFrame;
}

#endif
