#ifndef FA_PYTHON_H
#define FA_PYTHON_H

#include <string>

#include <misc/boost_python.h>

namespace FAWorld
{
    class Inventory;
}

namespace Engine
{
    class EngineMain;
}

namespace FAGui
{
    class GuiManager;

    class FAPythonFuncs
    {
        public:
            FAPythonFuncs(FAWorld::Inventory& playerInv, GuiManager& guiManager, Engine::EngineMain& engine):
                mPlayerInv(playerInv), mGuiManager(guiManager), mEngine(engine)
            {}

        protected:
            void showMainMenu();
            void quitGame();
            void pauseGame();
            void unpauseGame();
            void startGame();
            void saveGame();
            void loadGame();
            void playSound(const std::string& path);
            boost::python::list getHotkeyNames();
            boost::python::list getHotkeys();
            void setHotkey(std::string function, boost::python::list pyhotkey);
            void placeItem(uint32_t toPara, uint32_t fromPara, uint32_t fromY, uint32_t fromX,
                           uint32_t toY, uint32_t toX, uint32_t beltX);
            boost::python::dict updateInventory();
            bool canPlace(uint32_t toPara, uint32_t fromPara, uint32_t fromY, uint32_t fromX,
                          uint32_t toY, uint32_t toX, uint32_t beltX);
            std::string getInvClass();

            friend void init_module_freeablo();

            FAWorld::Inventory& mPlayerInv;
            GuiManager& mGuiManager;
            Engine::EngineMain& mEngine;
    };

    void initPython(FAPythonFuncs& funcs);
}

#endif
