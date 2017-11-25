#include "guimanager.h"

#include <string>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <iostream>

#include <misc/misc.h>
#include <serial/textstream.h>

#include "../faworld/world.h"
#include "../faworld/actorstats.h"
#include "../farender/renderer.h"
#include "../engine/enginemain.h"
#include "../faworld/player.h"
#include "../fasavegame/gameloader.h"

#include "boost/range/counting_range.hpp"
#include <boost/variant/variant.hpp>
#include "dialogmanager.h"

namespace FAGui
{
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
        mPentagramAnim.reset (new FARender::AnimationPlayer ());
        auto renderer = FARender::Renderer::get();
        mPentagramAnim->playAnimation(renderer->loadImage ("data/pentspn2.cel"), FAWorld::World::getTicksInPeriod(0.06f), FARender::AnimationPlayer::AnimationType::Looped);
    }

    GuiManager::~GuiManager()
    {
    }

    void GuiManager::nk_fa_begin_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, std::function<void(void)> action, bool isModal)
    {
        nk_style_push_vec2(ctx, &ctx->style.window.padding, nk_vec2(0,0));
        if (isModalDlgShown() && !isModal)
            flags |= NK_WINDOW_NO_INPUT;
        if (nk_begin(ctx, title, bounds, flags))
            {
                action();
            }

        nk_end(ctx);

        nk_style_pop_vec2(ctx);
    }

    void GuiManager::nk_fa_begin_image_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void(void)> action, bool isModal)
    {
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(background));
        nk_fa_begin_window(ctx, title, bounds, flags, action, isModal);
        nk_style_pop_style_item(ctx);
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

        // nk_widget_hovered without regard to activity of window
        bool nk_inactive_widget_is_hovered (nk_context *ctx)
        {
            return nk_input_is_mouse_hovering_rect(&ctx->input, nk_widget_bounds(ctx));
        }

        bool nk_widget_mouse_left (nk_context *ctx)
        {
            return !nk_inactive_widget_is_hovered(ctx) && nk_input_is_mouse_prev_hovering_rect(&ctx->input, nk_widget_bounds(ctx));
        }



        struct nk_rect alignRect (const struct nk_rect &inner_rect, const struct nk_rect &outer_rect, halign_t halign, valign_t valign) {
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

    void GuiManager::dialog(nk_context* ctx)
    {
        if (mDialogs.empty ())
            return;

        if (mCurRightPanel != PanelType::none)
            mCurRightPanel = PanelType::none;
        if (mCurLeftPanel != PanelType::none)
            mCurLeftPanel = PanelType::none;
        
        auto &activeDialog = mDialogs.back ();
        int dir = 0;
        if (nk_input_is_key_pressed (&ctx->input, NK_KEY_UP))
            dir = -1;
        if (nk_input_is_key_pressed (&ctx->input, NK_KEY_DOWN))
            dir = 1;

        // This is an escape key. Maybe later it should work through engineinputmanager
        if (nk_input_is_key_pressed (&ctx->input, NK_KEY_TEXT_RESET_MODE))
        {
           popDialogData ();
           if (mDialogs.empty ())
               return;
        }

        if (nk_input_is_key_pressed (&ctx->input, NK_KEY_ENTER))
        {
            activeDialog.mLines[activeDialog.selectedLine()].action ();
            return;
        }

        if (dir != 0)
        {
            int i = activeDialog.mSelectedLine;
            do
                {
                    i += dir;
                    if (i < 0)
                        i += activeDialog.mLines.size ();
                    else if (i >= static_cast<int> (activeDialog.mLines.size ()))
                        i -= activeDialog.mLines.size ();
                }
            while (!activeDialog.mLines[i].action);
            activeDialog.mSelectedLine = i;
        }

        auto renderer = FARender::Renderer::get();
        auto boxTex = renderer->loadImage("data/textbox2.cel");
        nk_flags flags = NK_WINDOW_NO_SCROLLBAR;
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        nk_fa_begin_image_window(ctx, "dialog", nk_rect(screenW / 2, screenH - boxTex->getHeight () - 153, boxTex->getWidth(), boxTex->getHeight ()), flags, boxTex->getNkImage(),
        [&]()
        {
           nk_layout_space_begin(ctx, NK_STATIC, 0, INT_MAX);
           auto cbRect = nk_rect (3, 3, boxTex->getWidth() - 6, boxTex->getHeight () - 6);
           nk_layout_space_push (ctx, cbRect);
           auto blackTex = renderer->loadImage("resources/black.png");
           {
               applyEffect effect (ctx, EffectType::checkerboarded);
               nk_image (ctx, nk_subimage_handle(blackTex->getNkImage().handle, blackTex->getWidth(), blackTex->getHeight(), cbRect));
           }

           int y = 5;
           constexpr int textRowHeight = 12;
           for (int i = 0; i < static_cast<int> (activeDialog.mLines.size ()); ++i)
           {
               auto &line = activeDialog.mLines[i];
               auto lineRect = nk_rect (3, 3 + y, boxTex->getWidth() - 6, textRowHeight);
               if (line.isSeparator)
               {   
                   auto separatorRect = nk_rect (3, 0, boxTex->getWidth() - 6, 3);
                   nk_layout_space_push (ctx, alignRect (separatorRect, lineRect, halign_t::center, valign_t::center));
                   auto separator_image = nk_subimage_handle(boxTex->getNkImage().handle, boxTex->getWidth(), boxTex->getHeight (), separatorRect);
                   nk_image (ctx, separator_image);
                   continue;
               }
               nk_layout_space_push (ctx, lineRect);
               if (nk_widget_is_mouse_click_down (ctx, NK_BUTTON_LEFT, true) && line.action)
                   {
                     activeDialog.mSelectedLine = i;
                     line.action ();
                     return;
                   }
               smallText (ctx, line.text.c_str (), line.color, (line.alignCenter ? NK_TEXT_ALIGN_CENTERED : NK_TEXT_ALIGN_LEFT) | NK_TEXT_ALIGN_MIDDLE);
               if (activeDialog.selectedLine() == i)
               {
                   auto pent = renderer->loadImage ("data/pentspn2.cel");
                   int pentOffset = 10;
                   auto textWidth = smallTextWidth (line.text.c_str ());
                   // left pentagram
                   {
                       int offset = 0;
                       if (line.alignCenter)
                           offset = ((boxTex->getWidth() - 6) / 2 - textWidth / 2 - pent->getWidth() - pentOffset);
                       nk_layout_space_push (ctx, nk_rect (3 + offset, lineRect.y, pent->getWidth(), pent->getHeight()));
                       nk_image (ctx, pent->getNkImage(mPentagramAnim->getCurrentFrame().second));
                   }
                   // right pentagram
                   {
                       int offset = textWidth + pentOffset;
                       if (line.alignCenter)
                           offset = ((boxTex->getWidth() - 6) / 2 + textWidth / 2 + pentOffset);
                       nk_layout_space_push (ctx, nk_rect (3 + offset, lineRect.y, pent->getWidth(), pent->getHeight()));
                       nk_image (ctx, pent->getNkImage(mPentagramAnim->getCurrentFrame().second));
                   }
                }
               
               y += textRowHeight;
           }
        }, true);
    }

    void GuiManager::updateAnimations()
    {
        for (auto &anim : {mPentagramAnim.get ()})
            anim->update();
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

            if (nk_button_label(ctx, "Save game"))
            {
                /*std::vector<uint8_t> streamData;
                Serial::WriteBitStream stream(streamData);
                FASaveGame::GameSaver saver(stream);

                FAWorld::World::get()->save(saver);*/

                Serial::TextWriteStream writeStream;
                FASaveGame::GameSaver saver(writeStream);

                FAWorld::World::get()->save(saver);

                /*writeStream->write(true);
                writeStream->write(false);

                writeStream->write(int64_t(900));
                writeStream->write(uint8_t(253));*/


                std::pair<uint8_t*, size_t> writtenData = writeStream.getData();

                /*std::string readData = (char*)writtenData.first;

                std::unique_ptr<Serial::ReadStreamInterface> readStream(new Serial::TextReadStream(readData));

                bool b1 = readStream->read_bool();
                bool b2 = readStream->read_bool();
                int64_t i1 = readStream->read_int64_t();
                uint8_t i2 = readStream->read_uint8_t();*/


                FILE* f = fopen("save.sav", "wb");
                fwrite(writtenData.first, 1, writtenData.second, f);
                fclose(f);
            }

            if (nk_button_label(ctx, "Quit"))
                engine.stop();
        }
        nk_end(ctx);

        nk_style_pop_style_item(ctx);
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
        nk_window_show (ctx, panelName (panelType), *panel (placement) == panelType ? NK_SHOWN : NK_HIDDEN);

        nk_fa_begin_image_window(ctx, panelName (panelType), dims, flags, invTex->getNkImage(), op, false);
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
            nk_layout_space_push(ctx, alignRect (nk_rect (0, 0, w, h), rect, halign_t::center, valign_t::center));
        }, [&](const struct nk_vec2 &point)
        {
             if (!item.isReal()) return;
            nk_layout_space_push(ctx, nk_rect (point.x, point.y, w, h));
            if (highlight == ItemHighlightInfo::highlightIfHover) {
                nk_button_label_styled (ctx, &dummyStyle, "");
                if (nk_inactive_widget_is_hovered(ctx))
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
                      if (nk_inactive_widget_is_hovered(ctx)) {
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
        FARender::FASpriteGroup* healthAndManaEmptyBulbs = renderer->loadImage("ctrlpan/p8bulbs.cel");

        int32_t bulbWidth = healthAndManaEmptyBulbs->getWidth();
        int32_t bulbHeight = healthAndManaEmptyBulbs->getHeight();

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

        int32_t healthBulbLeftOffset = 96;
        int32_t manaBulbLeftOffset = 464;

        // Centre the bottom menu on the bottom of the screen
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        struct nk_rect dims = nk_rect((screenW / 2) - (bottomMenuWidth / 2), screenH - bottomMenuHeight, bottomMenuWidth, bottomMenuHeight);

        Misc::ScopedSetter<float> setter(ctx->style.button.border, 0);
        Misc::ScopedSetter<struct nk_style_item> setter2(ctx->style.window.fixed_background, nk_style_item_color(nk_rgba(0,0,0,0)));

        nk_fa_begin_window(ctx, "bottom_menu", dims, NK_WINDOW_NO_SCROLLBAR, [&]()
        {
            nk_layout_space_begin(ctx, NK_STATIC, buttonHeight, INT_MAX);

            auto drawBgSection = [&](struct nk_rect rect)
            {
                struct nk_image section = nk_subimage_handle(bottomMenuTex->getNkImage(0).handle, bottomMenuWidth, bottomMenuHeight, rect);
                nk_layout_space_push(ctx, rect);
                nk_image(ctx, section);
            };

            // The bottom panel background image has full health and mana orbs baked into the image.
            // The empty orbs are contained in separate images, and include the background, clearly intended
            // to be drawn on top of the backgorund image. That works fine if you're a dumb cpu loop overwriting pixels,
            // but we're doing a normal draw with an alpha channel because it's not 1996 anymore. So, what we want is
            // the background image with empty health bulbs, on top of which we can draw the filled ones.
            // So, we draw the whole panel background excluding the bulbs, then separately draw the empty bulbs.

            // Leftmost section
            drawBgSection(nk_rect(0, 0, healthBulbLeftOffset, bottomMenuHeight));
            // Below left bulb section
            drawBgSection(nk_rect(healthBulbLeftOffset, bulbHeight, bulbWidth, bottomMenuHeight - bulbHeight));
            // Middle section
            drawBgSection(nk_rect(healthBulbLeftOffset + bulbWidth, 0, manaBulbLeftOffset - (healthBulbLeftOffset + bulbWidth), bottomMenuHeight));
            // Below right bulb section
            drawBgSection(nk_rect(manaBulbLeftOffset, bulbHeight, bulbWidth, bottomMenuHeight - bulbHeight));
            // Rightmost section
            drawBgSection(nk_rect(manaBulbLeftOffset + bulbWidth, 0, bottomMenuWidth - manaBulbLeftOffset, bottomMenuHeight));

            // Health bulb
            nk_layout_space_push(ctx, nk_rect(healthBulbLeftOffset, 0, bulbWidth, bulbHeight));
            nk_image(ctx, healthAndManaEmptyBulbs->getNkImage(0));

            // Mana bulb
            nk_layout_space_push(ctx, nk_rect(manaBulbLeftOffset, 0, bulbWidth, bulbHeight));
            nk_image(ctx, healthAndManaEmptyBulbs->getNkImage(1));


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

            auto drawBulb = [&] (float current, float max, int32_t leftOffset)
            {
                float percent = current / max;
                float useBulbHeight = bulbHeight * percent;

                struct nk_rect r = nk_rect(leftOffset, (bulbHeight - useBulbHeight), bulbWidth, useBulbHeight);
                nk_layout_space_push(ctx, r);

                struct nk_image bulbImage = nk_subimage_handle(bottomMenuTex->getNkImage(0).handle, bottomMenuWidth, bottomMenuHeight, r);
                nk_image(ctx, bulbImage);
            };

            FAWorld::ActorStats& stats = FAWorld::World::get()->getCurrentPlayer()->mStats;
            // draw current hp into health bulb
            drawBulb(stats.mHp.current, stats.mHp.max, healthBulbLeftOffset);
            // and current mana
            drawBulb(stats.mMana.current, stats.mMana.current, manaBulbLeftOffset);

            belt (ctx);
            descriptionPanel(ctx);

            nk_layout_space_end(ctx);
        }, false);
    }

    void GuiManager::menuText (nk_context *ctx, const char *text, nk_flags alignment) {
      FARender::Renderer* renderer = FARender::Renderer::get();
      nk_style_push_font(ctx, renderer->silverFont(24));
      nk_label(ctx, text, alignment);
       nk_style_pop_font(ctx);
    }

    void GuiManager::smallText (nk_context *ctx, const char *text, TextColor color, nk_flags alignment) {
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
      nk_label(ctx, text, alignment);
      nk_style_pop_color (ctx);
       nk_style_pop_font(ctx);
    }

    int GuiManager::smallTextWidth(const char* text)
    {
        auto renderer = FARender::Renderer::get();
        auto fnt = renderer->smallFont();
        return fnt->width(fnt->userdata, 0.0f, text, strlen (text) - 1);
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

        updateAnimations ();
         
        inventoryPanel(ctx);
        spellsPanel(ctx);
        questsPanel(ctx);
        characterPanel(ctx);
        bottomMenu(ctx);
        dialog(ctx);
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

    void GuiManager::popDialogData()
    {
        mDialogs.pop_back ();
    }

    void GuiManager::pushDialogData(DialogData&& data)
    {
        mDialogs.push_back (std::move (data));
    }

    bool GuiManager::isModalDlgShown() const
    {
        if (!mDialogs.empty ())
            return true;

        return false;
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
