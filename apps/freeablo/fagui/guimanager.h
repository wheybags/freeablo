#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <string>
#include <chrono>
#include <queue>
#include <functional>
#include "../engine/enginemain.h"
#include "textcolor.h"


#include <fa_nuklear.h>
#include "../faworld/inventory.h"
#include <boost/variant/variant_fwd.hpp>


namespace FARender
{
    class AnimationPlayer;
}

namespace Render
{
    enum class CursorHotspotLocation;
}

namespace FAWorld
{
    class Player;
}

namespace FAGui
{
    class GuiManager;
    class DialogData;

    enum class EffectType
    {
        none = 0,
        highlighted,
        checkerboarded,
    };

    // move all this to better place since cursor state is also dependent on spells etc.
    extern std::string cursorPath;
    extern Render::CursorHotspotLocation cursorHotspot;
    extern uint32_t cursorFrame;

    enum class PanelType
    {
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

    enum class ItemHighlightInfo
    {
        highlited,
        notHighlighed,
        highlightIfHover,
    };

    PanelPlacement panelPlacementByType(PanelType type);
    const char* bgImgPath(PanelType type);
    const char* panelName(PanelType type);

    class ScrollBox;

    class GuiManager
    {
        using self = GuiManager;
    public:
        GuiManager(Engine::EngineMain& engine, FAWorld::Player& player);
        ~GuiManager ();
        void nk_fa_begin_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, std::function<void()> action,
                                bool isModal);
        void nk_fa_begin_image_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background,
                                      std::function<void()> action, bool isModal);
        void dialog(nk_context* ctx);
        void updateAnimations();
        void update(bool paused, nk_context* ctx);
        void setDescription(std::string text, TextColor color = TextColor::white);
        void clearDescription();
        bool isInventoryShown() const;
        void popDialogData();
        void pushDialogData(DialogData &&data);
        // current support for modal dialogs seem to be non-existant, so here'll be some workarounds:
        bool isModalDlgShown () const;

    private:
        void togglePanel(PanelType type);
        template <class Function>
        void drawPanel(nk_context* ctx, PanelType panelType, Function op);
        void item(nk_context* ctx, FAWorld::EquipTarget target, boost::variant<struct nk_rect, struct nk_vec2> placement,
                  ItemHighlightInfo highligh);
        void inventoryPanel(nk_context* ctx);
        void characterPanel(nk_context* ctx);
        void questsPanel(nk_context* ctx);
        void spellsPanel(nk_context* ctx);
        void belt(nk_context* ctx);
        void bottomMenu(nk_context* ctx);
        void smallText(nk_context* ctx, const char* text, TextColor color = TextColor::white,
                       nk_flags alignment = NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);
        int smallTextWidth(const char* text);
        void descriptionPanel(nk_context* ctx);
        PanelType* panel(PanelPlacement placement);
        const PanelType* panel(PanelPlacement placement) const;

    private:
        Engine::EngineMain& mEngine;
        FAWorld::Player& mPlayer;
        std::string mDescription;
        TextColor mDescriptionColor = TextColor::white;
        PanelType mCurRightPanel = PanelType::none, mCurLeftPanel = PanelType::none;
        std::vector<DialogData> mDialogs;
        std::unique_ptr<FARender::AnimationPlayer> mPentagramAnim;
    };
}

#endif
