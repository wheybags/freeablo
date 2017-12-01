#include "mainmenu.h"
#include "fa_nuklear.h"
#include "guimanager.h"
#include "nkhelpers.h"

namespace FAGui
{
    MainMenuScreen::MainMenuScreen(const MainMenuHandler& menu) : mMenu(menu)
    {
    }

    MainMenuScreen::~MainMenuScreen()
    {
    }

    StartingScreen::StartingScreen(const MainMenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mSmLogo.reset(new FARender::AnimationPlayer());
        mSmLogo->playAnimation(renderer->loadImage("ui_art/smlogo.pcx&trans=0,255,0&vanim=154"), FAWorld::World::getTicksInPeriod(0.06f),
                               FARender::AnimationPlayer::AnimationType::Looped);
        mFocus.reset(new FARender::AnimationPlayer());
        mFocus->playAnimation(renderer->loadImage("ui_art/focus.pcx&trans=0,255,0&vanim=30"), FAWorld::World::getTicksInPeriod(0.06f),
                              FARender::AnimationPlayer::AnimationType::Looped);
    }

    void StartingScreen::update(nk_context* ctx)
    {
        for (auto ptr : {mSmLogo.get (), mFocus.get ()})
            ptr->update ();
        auto renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        Misc::ScopedSetter<float> setter(ctx->style.window.border, 0);
        auto bg = renderer->loadImage("ui_art/mainmenu.pcx")->getNkImage();
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(bg));
        if(nk_begin(ctx, "startingScreen",
                    nk_rect(screenW / 2 - MainMenuHandler::width / 2, screenH / 2 - MainMenuHandler::height / 2, MainMenuHandler::width,
                            MainMenuHandler::height),
                    NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
        {
            nk_layout_space_begin(ctx, NK_STATIC, 48, INT_MAX);
            {
                nk_layout_space_push(ctx, {125, 0, 390, 154});
                {
                    auto frame = mSmLogo->getCurrentFrame();
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                }

                int item_index = 0;
                auto add_item = [&](const char* text, const struct nk_rect& rect)
                {
                    nk_layout_space_push(ctx, rect);
                    menuText(ctx, text, MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                    if(active_item_index == item_index)
                    {
                        auto frame = mFocus->getCurrentFrame();
                        auto frameRect = nk_rect (0, 0, frame.first->getWidth(), frame.first->getHeight());
                        nk_layout_space_push (ctx, alignRect (frameRect, rect, halign_t::left, valign_t::center));
                        nk_image(ctx, frame.first->getNkImage(frame.second));
                        nk_layout_space_push (ctx, alignRect (frameRect, rect, halign_t::right, valign_t::center));
                        nk_image(ctx, frame.first->getNkImage(frame.second));
                    }
                    ++item_index;
                };
                add_item("Single Player", {65, 191, 510, 42});
                add_item("Multi Player", {65, 234, 510, 42});
                add_item("Replay Intro", {65, 276, 510, 42});
                add_item("Show Credits", {65, 319, 510, 42});
                add_item("Exit Diablo", {65, 361, 510, 42});

                nk_layout_space_push(ctx, {17, 442, 605, 21});
                menuText(ctx, "Freeablo", MenuFontColor::silver, 16, NK_TEXT_ALIGN_LEFT);
            }
            nk_layout_space_end(ctx);
        }
        nk_style_pop_style_item(ctx);
        nk_end(ctx);
    }

    MainMenuHandler::MainMenuHandler()
    {
    }

    void MainMenuHandler::update(nk_context* ctx) const
    {
        mActiveScreen->update(ctx);
    }

    void MainMenuScreen::menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment)
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
