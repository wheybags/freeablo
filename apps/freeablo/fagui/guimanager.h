#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "../faworld/itemmanager.h"
#include <string>
#include <chrono>


#include "fapython.h"

namespace FAGui
{

    class ScrollBox;
    class GuiManager
    {
        public:

        enum GuiType
        {
            TitleScreen,
            MainMenu,
            Credits,
            IngameMenu
        };

        GuiManager(FAWorld::Inventory &playerInventory, Engine::EngineMain& engine, std::string invClass);
        void showTitleScreen();
        void showIngameGui();
        void showMainMenu();
        void showCredits();
        void showSelectHeroMenu();
        void showChooseClassMenu();
        void showEnterNameMenu(int classNumber);
        void showInvalidNameMenu(int classNumber);
        void update(bool paused);
        GuiType currentGuiType() const;
        static std::string invClass;

        FAPythonFuncs mPythonFuncs;

    private:
        void updateGui(bool paused);
        void hideAllMenus();
        GuiType mCurrentGuiType;
        std::chrono::system_clock::time_point mStartTime;
        std::shared_ptr<ScrollBox> mCreditsScrollBox;
    };

    extern std::string cursorPath;

    extern uint32_t cursorFrame;
}

#endif
