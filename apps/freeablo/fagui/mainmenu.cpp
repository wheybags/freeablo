#include "mainmenu.h"
#include "fa_nuklear.h"
#include "guimanager.h"

namespace FAGui
{
    MainMenu::MainMenu()
    {
        mSmLogo.reset(new FARender::AnimationPlayer());
        auto renderer = FARender::Renderer::get();
        mSmLogo->playAnimation(renderer->loadImage("ui_art/smlogo.pcx&trans=0,255,0&vanim=154"), FAWorld::World::getTicksInPeriod(0.06f),
                               FARender::AnimationPlayer::AnimationType::Looped);
    }

    void MainMenu::update()
    {
        mSmLogo->update();
    }

    void MainMenu::startingScreen(nk_context* ctx)
    {
        auto renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        Misc::ScopedSetter<float> setter(ctx->style.window.border, 0);
        auto bg = renderer->loadImage("ui_art/mainmenu.pcx")->getNkImage();
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(bg));
        if(nk_begin(ctx, "startingScreen", nk_rect(screenW / 2 - menuWidth / 2, screenH / 2 - menuHeight / 2, menuWidth, menuHeight),
                    NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
        {
            nk_layout_space_begin(ctx, NK_STATIC, 48, INT_MAX);
            {
                nk_layout_space_push(ctx, {125, 0, 390, 154});
                auto frame = mSmLogo->getCurrentFrame();
                nk_image(ctx, frame.first->getNkImage(frame.second));

                nk_layout_space_push(ctx, {65, 191, 510, 42});
                menuText(ctx, "Single Player", MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                nk_layout_space_push(ctx, {65, 234, 510, 42});
                menuText(ctx, "Multi Player", MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                nk_layout_space_push(ctx, {65, 276, 510, 42});
                menuText(ctx, "Replay Intro", MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                nk_layout_space_push(ctx, {65, 319, 510, 42});
                menuText(ctx, "Show Credits", MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                nk_layout_space_push(ctx, {65, 361, 510, 42});
                menuText(ctx, "Exit Diablo", MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                nk_layout_space_push(ctx, {17, 442, 605, 21});
                menuText(ctx, "Freeablo", MenuFontColor::silver, 16, NK_TEXT_ALIGN_LEFT);
            }
            nk_layout_space_end(ctx);
        }
        nk_style_pop_style_item(ctx);
        nk_end(ctx);
    }

    void MainMenu::menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        switch(color)
        {
            case MenuFontColor::gold:
                nk_style_push_font(ctx, renderer->goldFont(fontSize));
                break;
            case MenuFontColor::silver:
                nk_style_push_font(ctx, renderer->silverFont(fontSize));
                break;
        }
        nk_label(ctx, text, textAlignment);
        nk_style_pop_font(ctx);
    }
}
