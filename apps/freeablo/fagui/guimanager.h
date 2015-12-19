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

        enum GuiType
        {
            TitleScreen,
            MainMenu,
            IngameMenu
        };

        GuiManager(FAWorld::Inventory &playerInventory, Engine::EngineMain& engine, std::string invClass);
        void showTitleScreen();
        void showIngameGui();
        void showMainMenu();
        void updateGui();
        GuiType currentGuiType() const;
        static std::string invClass;

        FAPythonFuncs mPythonFuncs;

        private:
            GuiType mCurrentGuiType;

    };

    extern std::string cursorPath;

    extern uint32_t cursorFrame;
}

#endif
