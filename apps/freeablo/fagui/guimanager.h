#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "../faworld/itemmanager.h"
#include <string>
#include <chrono>
#include <queue>
#include <functional>


#include <fa_nuklear.h>

namespace Rocket
{
namespace Core
{
class ElementDocument;
}
}

namespace FAGui
{

    class ScrollBox;
    class GuiManager
    {
    public:

        GuiManager(FAWorld::Inventory &playerInventory, Engine::EngineMain& engine, std::string invClass);

        void GuiManager::update(bool paused, nk_context* ctx);


    private:

        Engine::EngineMain& mEngine;
    };
}

#endif
