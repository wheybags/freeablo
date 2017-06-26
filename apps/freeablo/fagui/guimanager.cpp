#include "guimanager.h"

#include <string>

#include <input/hotkey.h>

#include "../faworld/world.h"
#include "../farender/renderer.h"
#include "../engine/threadmanager.h"
#include "../engine/enginemain.h"



namespace FAGui
{   
    std::map<std::string, Rocket::Core::ElementDocument*> menus;

    GuiManager::GuiManager(FAWorld::Inventory & playerInventory, Engine::EngineMain& engine, std::string invClass)
        : mEngine(engine)
    {

    }

    void nk_fa_begin_image_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void(void)> action)
    {
        nk_style_item tmpBg = ctx->style.window.fixed_background;
        struct nk_vec2 tmpPadding = ctx->style.window.padding;

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
                engine.unPause();

            if (nk_button_label(ctx, "Quit"))
                engine.stop();
        }
        nk_end(ctx);

        nk_style_pop_style_item(ctx);
    }

    void bottomMenu(nk_context* ctx)
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
                std::cout << "pressed CHAR" << std::endl;

            // QUEST button
            if (bottomMenuButton(buttonRow2TopIndent, buttonLeftIndent, questButtonFrame))
                std::cout << "pressed QUEST" << std::endl;

            // MAP button
            if (bottomMenuButton(buttonRow3TopIndent, buttonLeftIndent, mapButtonFrame))
                std::cout << "pressed MAP" << std::endl;

            // MENU button
            if (bottomMenuButton(buttonRow4TopIndent, buttonLeftIndent, menuButtonFrame))
                std::cout << "pressed MENU" << std::endl;

            // INV button
            if (bottomMenuButton(buttonRow1TopIndent, buttonRightIndent, invButtonFrame))
                std::cout << "pressed INV" << std::endl;

            // SPELLS button
            if (bottomMenuButton(buttonRow2TopIndent, buttonRightIndent, spellsButtonFrame))
                std::cout << "pressed SPELLS" << std::endl;

            nk_layout_space_end(ctx);
        });
    }

    void GuiManager::update(bool paused, nk_context* ctx)
    {
        bottomMenu(ctx);

        if (paused)
            pauseMenu(ctx, mEngine);
    }
}
