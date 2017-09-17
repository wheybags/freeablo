#include "guimanager.h"

#include <string>

#include <input/hotkey.h>

#include "../faworld/world.h"
#include "../farender/renderer.h"
#include "../engine/threadmanager.h"
#include "../engine/enginemain.h"
#include "../faworld/player.h"

#include "boost/range/counting_range.hpp"
#include <boost/variant/variant.hpp>

namespace FAGui
{
    std::map<std::string, Rocket::Core::ElementDocument*> menus;

    PanelPlacement panelPlacementByType(PanelType type) {
        switch (type)
        {
        case PanelType::none:      return PanelPlacement::none;
        case PanelType::inventory: return PanelPlacement::right;
        case PanelType::spells:    return PanelPlacement::right;
        case PanelType::character: return PanelPlacement::left;
        case PanelType::quests:    return PanelPlacement::left;
        }
        return PanelPlacement::none;
    }

    const char* bgImgPath(PanelType type)
    {
        switch (type)
        {
        case PanelType::none:      break;
        case PanelType::inventory: return "data/inv/inv.cel";
        case PanelType::spells:    return "data/spellbk.cel";
        case PanelType::character: return "data/char.cel";
        case PanelType::quests:    return "data/quest.cel";
        }
        return nullptr;
    }

    const char* panelName(PanelType type)
    {
        switch (type)
        {
        case PanelType::none: return "none";
        case PanelType::inventory: return "inventory";
        case PanelType::spells: return "spells";
        case PanelType::character: return "character";
        case PanelType::quests: return "quests";
        }
        return "";
    }

    GuiManager::GuiManager(Engine::EngineMain& engine, FAWorld::Player &player)
        : mEngine(engine), mPlayer (player)
    {

    }

    void nk_fa_begin_image_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void(void)> action)
    {
        nk_style_item tmpBg = ctx->style.window.fixed_background;
        struct nk_vec2 tmpPadding = ctx->style.window.padding;

        ctx->active = ctx->current;
        ctx->style.window.fixed_background = nk_style_item_image(background);
        ctx->style.window.padding = nk_vec2(0, 0);

        if (nk_begin(ctx, title, bounds, flags))
            action();

        nk_end(ctx);

        ctx->style.window.fixed_background = tmpBg;
        ctx->style.window.padding = tmpPadding;
    }

    void pauseMenu(nk_context* ctx, Engine::EngineMain& engine)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));

        if (nk_begin(ctx, "pause menu", nk_rect(0, 0, screenW, screenH), 0))
        {
            nk_layout_row_dynamic(ctx, 30, 1);

            nk_label(ctx, "PAUSED", NK_TEXT_CENTERED);

            if (nk_button_label(ctx, "Resume"))
                engine.togglePause();

            if (nk_button_label(ctx, "Quit"))
                engine.stop();
        }
        nk_end(ctx);

        nk_style_pop_style_item(ctx);
    }


    namespace
    {
        enum struct halign_t {
            left,
            center,
            right,
        };
        enum struct valign_t {
            top,
            center,
            bottom,
        };
        struct nk_vec2 center (const struct nk_rect &rect) {
            return {rect.x + rect.w / 2, rect.y + rect.h / 2};
        }

        bool nk_widget_is_mouse_click_down (nk_context *ctx, nk_buttons buttons, bool down)
        {
            return nk_widget_has_mouse_click_down (ctx, buttons, down) && ctx->input.mouse.buttons[buttons].clicked;
        }

        bool nk_widget_mouse_left (nk_context *ctx)
        {
            return !nk_widget_is_hovered(ctx) && nk_input_is_mouse_prev_hovering_rect(&ctx->input, nk_widget_bounds(ctx));
        }



        struct nk_rect align_rect (const struct nk_rect &inner_rect, const struct nk_rect &outer_rect, halign_t halign, valign_t valign) {
            auto c = center (outer_rect);
            auto shift = (outer_rect.w - inner_rect.w) / 2;
            switch (halign) {
                case halign_t::left: c.x -= shift; break;
                case halign_t::right: c.x += shift; break;
                default:
                  break;
            }
            shift = (outer_rect.h - inner_rect.h) / 2;
            switch (valign) {
                case valign_t::top: c.y -= shift; break;
                case valign_t::bottom: c.y += shift; break;
                default:
                  break;
            }
            return {c.x - inner_rect.w/2, c.y - inner_rect.h/2, inner_rect.w, inner_rect.h};
        }
    }

    template <typename Function>
    void GuiManager::drawPanel (nk_context* ctx, PanelType panelType, Function op)
    {
        auto renderer = FARender::Renderer::get();
        auto invTex = renderer->loadImage(bgImgPath (panelType));
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        auto placement = panelPlacementByType (panelType);
        struct nk_rect dims = nk_rect(
            [&]()
            {
            switch (placement)
            {
            case PanelPlacement::none: break;
            case PanelPlacement::left: return 0;
            case PanelPlacement::right: return screenW - invTex->getWidth();
            }
            return 0;
            }(), screenH - 125 - invTex->getHeight(),
            invTex->getWidth(), invTex->getHeight());
        nk_flags flags = NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND;
        if (*panel (placement) == panelType)
            nk_fa_begin_image_window(ctx, panelName (panelType), dims, flags, invTex->getNkImage(), op);
        else
            nk_window_close (ctx, panelName (panelType));
    }

    namespace {
        struct applyEffect {
          applyEffect (nk_context* ctx, EffectType type) : mCtx (ctx) {
              nk_set_user_data (mCtx, nk_handle_id (static_cast<int> (type)));
          }
           ~applyEffect () {
               nk_set_user_data (mCtx, nk_handle_id (static_cast<int> (EffectType::none)));
          }

           nk_context* mCtx;
        };
    }

    static nk_style_button dummyStyle = [](){
        static nk_style_button buttonStyle;
        buttonStyle.normal = buttonStyle.hover = buttonStyle.active = nk_style_item_hide();
        buttonStyle.border_color = {0, 0, 0, 0};
        buttonStyle.border = 0;
        buttonStyle.padding = {0, 0};
        return buttonStyle;
    }();
    void GuiManager::item (nk_context* ctx, FAWorld::EquipTarget target,
                           boost::variant<struct nk_rect, struct nk_vec2> placement, ItemHighlightInfo highlight)
    {
        auto &inv = mPlayer.getInventory ();
        using namespace FAWorld;
        if (!inv.getItemAt(MakeEquipTarget<Item::equipLoc::eqCURSOR> ()).isEmpty())
            highlight = ItemHighlightInfo::notHighlighed;
        bool checkerboarded = false;

        auto &item = inv.getItemAt (target);
        if (!item.isReal()) {
        if (item.getEquipLoc() == FAWorld::Item::equipLoc::eqTWOHAND && target.location == FAWorld::Item::equipLoc::eqRIGHTHAND)
            {
                checkerboarded = true;
                highlight = ItemHighlightInfo::notHighlighed;
            }
        else
          return;
        }

        auto renderer = FARender::Renderer::get();

        auto frame = item.getGraphicValue();
        auto imgPath = "data/inv/objcurs.cel";
        auto sprite = renderer->loadImage (imgPath);
        auto img = sprite->getNkImage (frame);
        auto w = sprite->getWidth(frame);
        auto h = sprite->getHeight(frame);
        bool isHighlighted = (highlight == ItemHighlightInfo::highlited);

        boost::apply_visitor(Misc::overload([&](const struct nk_rect &rect)
        {
            nk_layout_space_push(ctx, align_rect (nk_rect (0, 0, w, h), rect, halign_t::center, valign_t::center));
        }, [&](const struct nk_vec2 &point)
        {
             if (!item.isReal()) return;
            nk_layout_space_push(ctx, nk_rect (point.x, point.y, w, h));
            if (highlight == ItemHighlightInfo::highlightIfHover) {
                nk_button_label_styled (ctx, &dummyStyle, "");
                if (nk_widget_is_hovered(ctx))
                    {
                       isHighlighted = true;
                    }
                else if (nk_widget_mouse_left (ctx))
                   clearDescription ();
            }
        }), placement);
        auto effectType = isHighlighted ? EffectType::highlighted : EffectType::none;
        effectType = checkerboarded ? EffectType::checkerboarded : effectType;
        if (isHighlighted)
            setDescription(item.getName());
        applyEffect effect (ctx, effectType);
        nk_image (ctx, img);
    }

    void GuiManager::inventoryPanel(nk_context* ctx)
    {
        using namespace FAWorld;
        drawPanel (ctx, PanelType::inventory, [&]()
        {
           static std::vector<std::pair<EquipTarget, struct nk_rect>> slot_rects =
                  {{MakeEquipTarget<Item::equipLoc::eqHEAD>()    , nk_rect(133, 3, 56, 56)},
                   {MakeEquipTarget<Item::equipLoc::eqAMULET>()   , nk_rect(205, 32, 28, 28)},
                   {MakeEquipTarget<Item::equipLoc::eqBODY>()     , nk_rect(133, 75, 58, 87)},
                   {MakeEquipTarget<Item::equipLoc::eqLEFTHAND>() , nk_rect(18, 75, 56, 84)},
                   {MakeEquipTarget<Item::equipLoc::eqRIGHTHAND>(), nk_rect(249, 75, 56, 84)},
                   {MakeEquipTarget<Item::equipLoc::eqLEFTRING>() , nk_rect(47, 178, 28, 28)},
                   {MakeEquipTarget<Item::equipLoc::eqRIGHTRING>(), nk_rect(248, 178, 28, 28)}};
            nk_layout_space_begin(ctx, NK_STATIC, 0, INT_MAX);
            {
               auto &inv = mPlayer.getInventory ();
                for (auto &p : slot_rects)
                    {
                      nk_layout_space_push (ctx, p.second);
                      nk_button_label_styled (ctx, &dummyStyle, "");
                      if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
                          inv.itemSlotLeftMouseButtonDown (p.first);
                      auto highlight = ItemHighlightInfo::notHighlighed;
                      if (nk_widget_is_hovered(ctx)) {
                          highlight = ItemHighlightInfo::highlited;
                      }
                      else if (nk_widget_mouse_left (ctx))
                            clearDescription ();
                      item (ctx, p.first, p.second, highlight);
                    }
            }
            constexpr auto cellSize = 29;
            auto invTopLeft = nk_vec2 (17, 222);
            float invWidth = Inventory::inventoryWidth * cellSize;
            float invHeight = Inventory::inventoryHeight * cellSize;
            nk_layout_space_push(ctx, nk_recta (invTopLeft,
                {invWidth, invHeight}));
            auto &inv = mPlayer.getInventory ();
            nk_button_label_styled (ctx, &dummyStyle, "");
            if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
                {
                    inv.inventoryMouseLeftButtonDown(
                            (ctx->input.mouse.pos.x - invTopLeft.x - ctx->current->bounds.x)/invWidth,
                            (ctx->input.mouse.pos.y - invTopLeft.y - ctx->current->bounds.y)/invHeight);
                }

            for (auto row : boost::counting_range (0, Inventory::inventoryHeight))
                for (auto col : boost::counting_range (0, Inventory::inventoryWidth))
                    {
                        auto cell_top_left = nk_vec2 (17 + col * cellSize, 222 + row * cellSize);
                        item (ctx, MakeEquipTarget<Item::equipLoc::eqINV> (col, row), cell_top_left, ItemHighlightInfo::highlightIfHover);
                    }
            nk_layout_space_end(ctx);
        });
    }

     void GuiManager::characterPanel(nk_context* ctx)
    {
        drawPanel (ctx, PanelType::character, [&](){});
    }

   void GuiManager::questsPanel(nk_context* ctx)
    {
        drawPanel (ctx, PanelType::quests, [&](){});
    }

    void GuiManager::spellsPanel(nk_context* ctx)
    {
        drawPanel (ctx, PanelType::spells, [&](){});
    }


    void GuiManager::belt(nk_context* ctx) {
        auto beltTopLeft = nk_vec2 (205, 21);
        auto beltWidth = 232.0f, beltHeight = 29.0f, cellSize = 29.0f;
        nk_layout_space_push(ctx, nk_recta (beltTopLeft,
                {beltWidth, beltHeight}));
            auto &inv = mPlayer.getInventory ();
            if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
                {
                    inv.beltMouseLeftButtonDown(
                            (ctx->input.mouse.pos.x - beltTopLeft.x - ctx->current->bounds.x)/beltWidth);
                }

            using namespace FAWorld;
            for (auto num : boost::counting_range (0, Inventory::beltWidth))
               {
                   auto cell_top_left = nk_vec2 (beltTopLeft.x + num * cellSize, beltTopLeft.y);
                   item (ctx, MakeEquipTarget<Item::equipLoc::eqBELT> (num), cell_top_left, ItemHighlightInfo::highlightIfHover);
               }
            nk_layout_space_end(ctx);
    }

    void GuiManager::bottomMenu(nk_context* ctx)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        // The bottom menu is made of two sprites: panel8.cel, which is the background,
        // and panel8bu.cel, which contains overlays for each button. It's pretty primitive,
        // the buttons are baked into the background image.
        FARender::FASpriteGroup* bottomMenuTex = renderer->loadImage("ctrlpan/panel8.cel");
        FARender::FASpriteGroup* bottomMenuButtonsTex = renderer->loadImage("ctrlpan/panel8bu.cel");

        int32_t bottomMenuWidth = bottomMenuTex->getWidth();
        int32_t bottomMenuHeight = bottomMenuTex->getHeight();

        // Magic numbers defining the positions of the buttons on the background
        int32_t buttonLeftIndent    = 10;
        int32_t buttonRightIndent   = 560;

        int32_t buttonWidth         = 71;
        int32_t buttonHeight        = 19;

        int32_t buttonRow1TopIndent = 25;
        int32_t buttonRow2TopIndent = 52;
        int32_t buttonRow3TopIndent = 92;
        int32_t buttonRow4TopIndent = 118;

        // indices into panel8bu.cel
        int32_t charButtonFrame     = 0;
        int32_t questButtonFrame    = 1;
        int32_t mapButtonFrame      = 2;
        int32_t menuButtonFrame     = 3;
        int32_t invButtonFrame      = 4;
        int32_t spellsButtonFrame   = 5;

        // Centre the bottom menu on the bottom of the screen
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        struct nk_rect dims = nk_rect((screenW / 2) - (bottomMenuWidth / 2), screenH - bottomMenuHeight, bottomMenuWidth, bottomMenuHeight);

        nk_fa_begin_image_window(ctx, "bottom_menu", dims, NK_WINDOW_NO_SCROLLBAR, bottomMenuTex->getNkImage(), [&]()
        {
            nk_layout_space_begin(ctx, NK_STATIC, buttonHeight, INT_MAX);

            nk_style_button buttonStyle = ctx->style.button;
            // The "unpressed" version of the button is baked into the background image, so just draw nothing
            buttonStyle.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
            buttonStyle.hover  = nk_style_item_color(nk_rgba(0, 0, 0, 0));

            auto bottomMenuButton = [&](int32_t topIndent, int32_t leftIndent, int32_t frame)
            {
                nk_layout_space_push(ctx, nk_rect(leftIndent, topIndent, buttonWidth, buttonHeight));
                buttonStyle.active = nk_style_item_image(bottomMenuButtonsTex->getNkImage(frame));
                return nk_button_label_styled(ctx, &buttonStyle, "");
            };

            // CHAR button
            if (bottomMenuButton(buttonRow1TopIndent, buttonLeftIndent, charButtonFrame))
                togglePanel (PanelType::character);

            // QUEST button
            if (bottomMenuButton(buttonRow2TopIndent, buttonLeftIndent, questButtonFrame))
                togglePanel (PanelType::quests);

            // MAP button
            if (bottomMenuButton(buttonRow3TopIndent, buttonLeftIndent, mapButtonFrame))
                std::cout << "pressed MAP" << std::endl;

            // MENU button
            if (bottomMenuButton(buttonRow4TopIndent, buttonLeftIndent, menuButtonFrame))
                std::cout << "pressed MENU" << std::endl;

            // INV button
            if (bottomMenuButton(buttonRow1TopIndent, buttonRightIndent, invButtonFrame))
                togglePanel (PanelType::inventory);

            // SPELLS button
            if (bottomMenuButton(buttonRow2TopIndent, buttonRightIndent, spellsButtonFrame))
                togglePanel (PanelType::spells);

            belt (ctx);
            descriptionPanel(ctx);
            nk_layout_space_end(ctx);
        });
    }

    void GuiManager::smallText (nk_context *ctx, const char *text, TextColor color) {
      FARender::Renderer* renderer = FARender::Renderer::get();
      nk_style_push_font(ctx, renderer->smallFont());
      nk_style_push_color (ctx, &ctx->style.text.color, [color]()
      {
          // Warning: These colors just placeholder similar colors (except white obviously),
          // To achieve real Diablo palette coloring of smaltext.cel we need to apply palette shift
          // which could not be represented as color multiplication so it's bettter to
          // probably generate separate textures in the time of decoding.
          switch (color) {
              case TextColor::white:  return nk_color{255, 255, 255, 255};
              case TextColor::blue:   return nk_color{170, 170, 255, 255};
              case TextColor::golden: return nk_color{225, 225, 155, 255};
              case TextColor::red:    return nk_color{255, 128, 128, 255};
          }
          return nk_color{255, 255, 255, 255};
      }());
      nk_label(ctx, text, NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);
      nk_style_pop_color (ctx);
       nk_style_pop_font(ctx);
    }

    void GuiManager::descriptionPanel (nk_context *ctx)
    {
      nk_layout_space_push(ctx, nk_rect (185, 66, 275, 60));
      smallText (ctx, mDescription.c_str (), mDescriptionColor);
    }

    void GuiManager::update(bool paused, nk_context* ctx)
    {
        if (paused)
            pauseMenu(ctx, mEngine);

        inventoryPanel(ctx);
        spellsPanel(ctx);
        questsPanel(ctx);
        characterPanel(ctx);
        bottomMenu(ctx);
    }

    void GuiManager::setDescription(std::string text, TextColor color)
    {
      mDescription = text;
      mDescriptionColor = color;
    }

    void GuiManager::clearDescription()
    {
        return setDescription("");
    }

    PanelType *GuiManager::panel (PanelPlacement placement) {
        switch (placement) {
        case PanelPlacement::none: break;
        case PanelPlacement::left: return &mCurLeftPanel;
        case PanelPlacement::right: return &mCurRightPanel;
        }
        return nullptr;
    }

    const PanelType* GuiManager::panel(PanelPlacement placement) const
    {
        return const_cast<self *> (this)->panel (placement);
    }

    bool GuiManager::isInventoryShown() const
    {
        return *panel (panelPlacementByType (PanelType::inventory)) == PanelType::inventory;
    }

    void GuiManager::togglePanel(PanelType type)
    {
        auto &curPanel = *panel (panelPlacementByType (type));
        if (curPanel == type)
            curPanel = PanelType::none;
        else
            curPanel = type;
    }

    std::string cursorPath = "data/inv/objcurs.cel";
    uint32_t cursorFrame = 0;
    Render::CursorHotspotLocation cursorHotspot = Render::CursorHotspotLocation::topLeft;
}
