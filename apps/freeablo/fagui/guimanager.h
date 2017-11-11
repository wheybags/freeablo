#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <string>
#include <chrono>
#include <queue>
#include <functional>
#include "../engine/enginemain.h"


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

    enum class TextColor
    {
        white,
        blue,
        golden,
        red,
    };

    class DialogLineData
    {
    public:
        DialogLineData& setAction(std::function<void ()> actionArg)
        {
            action = actionArg;
            return *this;
        }

    public:
        std::function<void ()> action;
        std::string text;
        bool alignCenter = false;
        bool isSeparator = false;
        TextColor color = TextColor::white;
    };

    class DialogData
    {
    public:
        DialogLineData& text_lines(const std::vector<std::string>& texts, TextColor color = TextColor::white, bool alignCenter = true)
        {
            auto& ret = mLines[mLastLine];
            for(auto& text : texts)
            {
                mLines[mLastLine].text = text;
                mLines[mLastLine].color = color;
                mLines[mLastLine].isSeparator = false;
                mLines[mLastLine].alignCenter = alignCenter;
                ++mLastLine;
            }
            skip_line();
            return ret;
        }

        void skip_line(int cnt = 1) { mLastLine += cnt; }

        void separator()
        {
            mLines[mLastLine].isSeparator = true;
            ++mLastLine;
            skip_line();
        }

        void header(const std::vector<std::string>& text)
        {
            for(auto& line : text)
            {
                text_lines({line}, TextColor::golden);
            }
            separator();
            skip_line();
        }

        int selectedLine()
        {
            if(mSelectedLine == -1)
                {
                    auto it = std::find_if (mLines.begin (), mLines.end (),
                        [](const DialogLineData &data){ return !!data.action; });
                        if(it != mLines.end ())
                            return mSelectedLine = it - mLines.begin ();
                    assert (false);
                }
            return mSelectedLine;
        }

    private:
        std::array<DialogLineData, 24> mLines;
        int mLastLine = 1;
        int mSelectedLine = -1;
        friend class FAGui::GuiManager;
    };

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
        void dialog(nk_context* ctx);
        void updateAnimations();
        void update(bool paused, nk_context* ctx);
        void setDescription(std::string text, TextColor color = TextColor::white);
        void clearDescription();
        bool isInventoryShown() const;

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
        boost::optional<DialogData> mActiveDialog;
        std::unique_ptr<FARender::AnimationPlayer> mPentagramAnim;
    };
}

#endif
