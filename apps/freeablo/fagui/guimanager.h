#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <string>
#include <chrono>
#include <queue>
#include <functional>
#include "../engine/enginemain.h"


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

        GuiManager(Engine::EngineMain& engine);

        void update(bool paused, nk_context* ctx);


    private:

        Engine::EngineMain& mEngine;
    };
}

#endif
