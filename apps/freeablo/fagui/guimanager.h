#pragma once
#include "textcolor.h"
#include <boost/variant/variant_fwd.hpp>
#include <chrono>
#include <fa_nuklear.h>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include "../engine/inputobserverinterface.h"
#include <boost/variant/variant_fwd.hpp>
#include <fa_nuklear.h>
#include <memory>
#include "dialogmanager.h"

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
    class EquipTarget;
    class Player;
    class World;
    class HoverStatus;
    class Item;
}

namespace FAGui
{
    class GuiManager;
    class DialogData;
    class MenuHandler;

    enum class EffectType
    {
        none = 0,
        highlighted,
        checkerboarded,
    };

    // move all this to better place since cursor state is also dependent on spells etc.
    extern std::string cursorPath;
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

    class ScopedApplyEffect
    {
    public:
        ScopedApplyEffect(nk_context* ctx, EffectType type) : mCtx(ctx) { nk_set_user_data(mCtx, nk_handle_id(static_cast<int>(type))); }
        ~ScopedApplyEffect() { nk_set_user_data(mCtx, nk_handle_id(static_cast<int>(EffectType::none))); }

        nk_context* mCtx;
    };

    PanelPlacement panelPlacementByType(PanelType type);
    const char* bgImgPath(PanelType type);
    const char* panelName(PanelType type);

    class ScrollBox;

    class GuiManager : public Engine::KeyboardInputObserverInterface
    {
        using self = GuiManager;

    public:
        GuiManager(Engine::EngineMain& engine);
        ~GuiManager();

        void update(bool inGame, bool paused, nk_context* ctx, const FAWorld::HoverStatus& hoverStatus);

        bool isInventoryShown() const;
        void popDialogData();
        void pushDialogData(DialogData&& data);
        // so gold split dialog blocks pause but allows you to move around, that's why it should be separate function
        bool isPauseBlocked() const;
        // current support for modal dialogs seem to be non-existant, so here'll be some workarounds:
        bool isModalDlgShown() const;
        void setPlayer(FAWorld::Player* player);
        bool isLastWidgetHovered(nk_context* ctx) const;

        static void smallText(nk_context* ctx, const char* text, TextColor color, nk_flags alignment);
        static void smallText(nk_context* ctx, const char* text, TextColor color = TextColor::white);

        void nk_fa_begin_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, std::function<void()> action, bool isModal);
        void nk_fa_begin_image_window(
            nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void()> action, bool isModal);

    private:


        void dialog(nk_context* ctx);
        void updateAnimations();
        void startingScreen();
        void togglePanel(PanelType type);
        void drawPanel(nk_context* ctx, PanelType panelType, std::function<void(void)> op);
        void triggerItem(const FAWorld::EquipTarget& target);
        void item(nk_context* ctx, FAWorld::EquipTarget target, boost::variant<struct nk_rect, struct nk_vec2> placement, ItemHighlightInfo highligh);
        void inventoryPanel(nk_context* ctx);
        void fillTextField(nk_context* ctx, float x, float y, float width, const char* text, TextColor color = TextColor::white);
        void characterPanel(nk_context* ctx);
        void questsPanel(nk_context* ctx);
        void spellsPanel(nk_context* ctx);
        void belt(nk_context* ctx);
        void bottomMenu(nk_context* ctx, const FAWorld::HoverStatus& hoverStatus);

        int smallTextWidth(const char* text);
        void descriptionPanel(nk_context* ctx, const std::string& description);
        PanelType* getPanelAtLocation(PanelPlacement placement);
        const PanelType* getPanelAtLocation(PanelPlacement placement) const;
        void notify(Engine::KeyboardInputAction action) override;
        void keyPress(const Input::Hotkey&) override;

    public:
        DialogManager mDialogManager;

//    private:
        Engine::EngineMain& mEngine;
        FAWorld::Player* mPlayer = nullptr;
        std::string mHoveredInventoryItemText;
        PanelType mCurRightPanel = PanelType::none, mCurLeftPanel = PanelType::none;
        std::vector<DialogData> mDialogs;
        std::unique_ptr<FARender::AnimationPlayer> mSmallPentagram;
        std::unique_ptr<MenuHandler> mMenuHandler;
        const FAWorld::Item* mGoldSplitTarget = nullptr;
        int mGoldSplitCnt = 0;
        bool mSkipDialogFrame = false;
    };
}
