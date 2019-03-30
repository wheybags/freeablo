#include "startingmenuscreen.h"
#include "../../engine/enginemain.h"
#include "../../farender/animationplayer.h"
#include "../../faworld/world.h"
#include "../menuhandler.h"
#include "../nkhelpers.h"
#include "multiplayermenu.h"
#include "selectheromenuscreen.h"

namespace FAGui
{
    StartingMenuScreen::StartingMenuScreen(MenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mFocus42.reset(new FARender::AnimationPlayer());
        mFocus42->playAnimation(renderer->loadImage("ui_art/focus42.pcx&trans=0,255,0&vanim=42"),
                                FAWorld::World::getTicksInPeriod("0.06"),
                                FARender::AnimationPlayer::AnimationType::Looped);
        mSmLogo = menu.createSmLogo();

        auto drawItem = [&](const char* text, const struct nk_rect& rect) {
            return [=](nk_context* ctx, bool isActive) {
                nk_layout_space_push(ctx, rect);
                menuText(ctx, text, MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                auto ret = DrawFunctionResult::noAction;
                if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT))
                {
                    ret = DrawFunctionResult::executeAction;
                }
                if (isActive)
                {
                    auto frame = mFocus42->getCurrentFrame();
                    auto frameRect = nk_rect(0, 0, frame.first->getWidth(), frame.first->getHeight());
                    nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::left, valign_t::center));
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                    nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::right, valign_t::center));
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                }
                return ret;
            };
        };
        mMenuItems.push_back({drawItem("Single Player", {65, 192, 510, 42}), [this]() {
                                  mMenuHandler.setActiveScreen<SelectHeroMenuScreen>();
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Multi Player", {65, 235, 510, 42}), [this]() {
                                  mMenuHandler.setActiveScreen<MultiplayerMenu>();
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Replay Intro", {65, 277, 510, 42}), []() { return ActionResult::continueDrawing; }});
        mMenuItems.push_back({drawItem("Show Credits", {65, 320, 510, 42}), []() { return ActionResult::continueDrawing; }});
        mRejectAction = [this]() {
            mMenuHandler.engine().stop();
            return ActionResult::stopDrawing;
        };
        mMenuItems.push_back({drawItem("Exit Diablo", {65, 363, 510, 42}), mRejectAction});
    }

    void StartingMenuScreen::menuItems(nk_context* ctx)
    {
        nk_layout_space_begin(ctx, NK_STATIC, 48, INT_MAX);
        {
            nk_layout_space_push(ctx, {125, 0, 390, 154});
            {
                auto frame = mSmLogo->getCurrentFrame();
                nk_image(ctx, frame.first->getNkImage(frame.second));
            }

            if (drawMenuItems(ctx) == ActionResult::stopDrawing)
                return;
            nk_layout_space_push(ctx, {17, 442, 605, 21});
            menuText(ctx, "Freeablo", MenuFontColor::silver, 16, NK_TEXT_ALIGN_LEFT);
        }
        nk_layout_space_end(ctx);
    }

    void StartingMenuScreen::update(nk_context* ctx)
    {
        for (auto ptr : {mSmLogo.get(), mFocus42.get()})
            ptr->update();
        auto renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        Misc::ScopedSetter<float> setter(ctx->style.window.border, 0);
        auto bg = renderer->loadImage("ui_art/mainmenu.pcx")->getNkImage();
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(bg));
        if (nk_begin(
                ctx,
                "startingScreen",
                nk_rect(screenW / 2 - MenuHandler::menuWidth / 2, screenH / 2 - MenuHandler::menuHeight / 2, MenuHandler::menuWidth, MenuHandler::menuHeight),
                NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
            menuItems(ctx);
        nk_end(ctx);
        nk_style_pop_style_item(ctx);
    }
}
