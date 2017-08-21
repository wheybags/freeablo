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
    enum class PanelType {
        none,
        inventory,
        spells,
        character,
        quests,
    };

    enum class PanelPlacement
    {
        none,
        left,
        right,
    };

    PanelPlacement panelPlacementByType (PanelType type);
    const char *bgImgPath (PanelType type);
    const char *panelName (PanelType type);

    class ScrollBox;
    class GuiManager
    {
    public:
        GuiManager(Engine::EngineMain& engine);
        void update(bool paused, nk_context* ctx);

    private:
        void togglePanel (PanelType type);
        template <class Function>
        void drawPanel(nk_context* ctx, PanelType panelType, Function op);
        void inventoryPanel(nk_context* ctx);
        void characterPanel(nk_context* ctx);
        void questsPanel(nk_context* ctx);
        void spellsPanel(nk_context* ctx);
        void bottomMenu(nk_context* ctx);
        PanelType* panel(PanelPlacement placement);

    private:
        Engine::EngineMain& mEngine;
        PanelType mCurRightPanel, mCurLeftPanel;
    };
}

#endif
