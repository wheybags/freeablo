#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "textcolor.h"
#include <chrono>
#include <functional>
#include <queue>
#include <string>

#include <boost/variant/variant_fwd.hpp>
#include <fa_nuklear.h>
#include <memory>

struct nk_context;
typedef uint32_t nk_flags;
struct nk_rect;
struct nk_vec2;
struct nk_image;

namespace Engine
{
    class EngineMain;
}

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
    struct EquipTarget;
    class Player;
}

namespace FAGui
{
    class GuiManager;
    class DialogData;
    class MainMenuHandler;

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
        ~GuiManager();
        void nk_fa_begin_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, std::function<void()> action, bool isModal);
        void nk_fa_begin_image_window(
            nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void()> action, bool isModal);
        void dialog(nk_context* ctx);
        void updateAnimations();
        void updateGameUI(bool paused, nk_context* ctx);
        void updateMenuUI(nk_context* ctx);
        void setDescription(std::string text, TextColor color = TextColor::white);
        void clearDescription();
        bool isInventoryShown() const;
        void popDialogData();
        void pushDialogData(DialogData&& data);
        // current support for modal dialogs seem to be non-existant, so here'll be some workarounds:
        bool isModalDlgShown() const;
        void startingScreen();

    private:
        void togglePanel(PanelType type);
        template <class Function> void drawPanel(nk_context* ctx, PanelType panelType, Function op);
        void item(nk_context* ctx, FAWorld::EquipTarget target, boost::variant<struct nk_rect, struct nk_vec2> placement, ItemHighlightInfo highligh);
        void inventoryPanel(nk_context* ctx);
        void characterPanel(nk_context* ctx);
        void questsPanel(nk_context* ctx);
        void spellsPanel(nk_context* ctx);
        void belt(nk_context* ctx);
        void bottomMenu(nk_context* ctx);
        static void bigTGoldText(nk_context* ctx, const char* text, nk_flags alignment);
        static void smallText(nk_context* ctx, const char* text, TextColor color, nk_flags alignment);
        static void smallText(nk_context* ctx, const char* text, TextColor color = TextColor::white);
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
        std::unique_ptr<MainMenuHandler> mMainMenuHandler;
    };
}

#endif
