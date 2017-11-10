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

namespace Rocket
{
namespace Core
{
class ElementDocument;
}
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
        std::string text;
        bool alignCenter = false;
        bool isSeparator = false;
        TextColor color = TextColor::white;
    };
    
    class DialogData
    {
    public:
        void text_lines (const std::vector<std::string> &texts, TextColor color = TextColor::white, bool alignCenter = true)
        {
            for (auto &text : texts)
            {
                mLines[last_line].text = text;
                mLines[last_line].color = color;
                mLines[last_line].isSeparator = false;
                mLines[last_line].alignCenter = alignCenter;
                ++last_line;
            }
            skip_line ();
        }
        void skip_line (int cnt = 1) { last_line += cnt; }
        void separator () { mLines[last_line].isSeparator = true; ++last_line; skip_line (); }
        void header (const std::vector<std::string> &text)
        {
          for (auto &line : text)
          {
              text_lines ({line}, TextColor::golden);
          }
          separator ();
          skip_line ();
        }
    
    private:
        std::array<DialogLineData, 24> mLines;
        int last_line = 1;
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

    enum class ItemHighlightInfo {
        highlited,
        notHighlighed,
        highlightIfHover,
    };

    enum class DescriptionType {
        none,
        item,
    };

    PanelPlacement panelPlacementByType (PanelType type);
    const char *bgImgPath (PanelType type);
    const char *panelName (PanelType type);

    class ScrollBox;
    class GuiManager
    {
        using self = GuiManager;
    public:
        GuiManager(Engine::EngineMain& engine, FAWorld::Player &player);
        void dialog(nk_context* ctx);
        void update(bool paused, nk_context* ctx);
        void setDescription (std::string text, TextColor color = TextColor::white);
        void clearDescription();
        bool isInventoryShown () const;

    private:
        void togglePanel (PanelType type);
        template <class Function>
        void drawPanel(nk_context* ctx, PanelType panelType, Function op);
        void item(nk_context* ctx, FAWorld::EquipTarget target, boost::variant<struct nk_rect, struct nk_vec2> placement, ItemHighlightInfo highligh);
        void inventoryPanel(nk_context* ctx);
        void characterPanel(nk_context* ctx);
        void questsPanel(nk_context* ctx);
        void spellsPanel(nk_context* ctx);
        void belt(nk_context* ctx);
        void bottomMenu(nk_context* ctx);
        void smallText(nk_context* ctx, const char* text, TextColor color = TextColor::white, nk_flags alignment = NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);
        void descriptionPanel(nk_context* ctx);
        PanelType* panel(PanelPlacement placement);
        const PanelType* panel(PanelPlacement placement) const;

    private:
        Engine::EngineMain& mEngine;
        FAWorld::Player& mPlayer;
        std::string mDescription;
        TextColor mDescriptionColor;
        DescriptionType mDescriptionType; 
        PanelType mCurRightPanel = PanelType::none, mCurLeftPanel = PanelType::none;
        boost::optional<DialogData> mActiveDialog;
    };
}

#endif
