#include "mainmenu.h"
#include "fa_nuklear.h"
#include "guimanager.h"
#include "nkhelpers.h"
#include "../farender/renderer.h"
#include "../farender/animationplayer.h"
#include "../engine/enginemain.h"

namespace FAGui
{
    MainMenuScreen::MainMenuScreen(MainMenuHandler& menu) : mMenuHandler(menu)
    {
    }

    MainMenuScreen::~MainMenuScreen()
    {
    }

    StartingScreen::StartingScreen(MainMenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mSmLogo.reset(new FARender::AnimationPlayer());
        mSmLogo->playAnimation(renderer->loadImage("ui_art/smlogo.pcx&trans=0,255,0&vanim=154"), FAWorld::World::getTicksInPeriod(0.06f),
                               FARender::AnimationPlayer::AnimationType::Looped);
        mFocus42.reset(new FARender::AnimationPlayer());
        mFocus42->playAnimation(renderer->loadImage("ui_art/focus42.pcx&trans=0,255,0&vanim=42"), FAWorld::World::getTicksInPeriod(0.06f),
                              FARender::AnimationPlayer::AnimationType::Looped);
    }

    void StartingScreen::update(nk_context* ctx)
    {
        for (auto ptr : {mSmLogo.get (), mFocus42.get ()})
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

                int itemIndex = 0;
                auto add_item = [&](const char* text, const struct nk_rect& rect, std::function<void ()> action)
                {
                    nk_layout_space_push(ctx, rect);
                    menuText(ctx, text, MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                    if(activeItemIndex == itemIndex)
                    {
                        if (nk_input_is_key_pressed (&ctx->input, NK_KEY_ENTER))
                            action ();
                        auto frame = mFocus42->getCurrentFrame();
                        auto frameRect = nk_rect (0, 0, frame.first->getWidth(), frame.first->getHeight());
                        nk_layout_space_push (ctx, alignRect (frameRect, rect, halign_t::left, valign_t::center));
                        nk_image(ctx, frame.first->getNkImage(frame.second));
                        nk_layout_space_push (ctx, alignRect (frameRect, rect, halign_t::right, valign_t::center));
                        nk_image(ctx, frame.first->getNkImage(frame.second));
                    }
                    ++itemIndex;
                };
                add_item("Single Player", {65, 192, 510, 42}, [this](){ mMenuHandler.startGame (); });
                add_item("Multi Player", {65, 235, 510, 42}, [](){});
                add_item("Replay Intro", {65, 277, 510, 42}, [](){});
                add_item("Show Credits", {65, 320, 510, 42}, [](){});
                add_item("Exit Diablo", {65, 363, 510, 42}, [this](){ mMenuHandler.quit ();});
                if (nk_input_is_key_pressed (&ctx->input, NK_KEY_UP))
                    --activeItemIndex;
                if (nk_input_is_key_pressed (&ctx->input, NK_KEY_DOWN))
                    ++activeItemIndex;
                activeItemIndex = (activeItemIndex + itemIndex) % itemIndex;

                nk_layout_space_push(ctx, {17, 442, 605, 21});
                menuText(ctx, "Freeablo", MenuFontColor::silver, 16, NK_TEXT_ALIGN_LEFT);
            }
            nk_layout_space_end(ctx);
        }
        nk_style_pop_style_item(ctx);
        nk_end(ctx);
    }

    MainMenuHandler::MainMenuHandler(Engine::EngineMain& engine) : mEngine (engine)
    {
    }

    void MainMenuHandler::update(nk_context* ctx) const
    {
        mActiveScreen->update(ctx);
    }

    void MainMenuHandler::quit()
    {
        mEngine.stop();
    }

    void MainMenuHandler::startGame()
    {
        mEngine.startGame();
    }

    void MainMenuScreen::menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        nk_style_push_color (ctx, &ctx->style.text.color, nk_color{255, 255, 255, 255});
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
        nk_style_pop_color (ctx);
        nk_style_pop_font(ctx);
    }
}
