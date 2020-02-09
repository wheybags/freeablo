#include "pausemenuscreen.h"
#include "../../engine/enginemain.h"
#include "../../farender/animationplayer.h"
#include "../../farender/renderer.h"
#include "../../fasavegame/gameloader.h"
#include "../../faworld/world.h"
#include "../menuhandler.h"
#include "../nkhelpers.h"
#include "serial/textstream.h"
#include <cstring>

namespace FAGui
{

    PauseMenuScreen::PauseMenuScreen(MenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mBigPentagram.reset(new FARender::AnimationPlayer());
        auto pentImg = renderer->loadImage("data/pentspin.cel");
        mBigPentagram->playAnimation(pentImg, FAWorld::World::getTicksInPeriod("0.06"), FARender::AnimationPlayer::AnimationType::Looped);
        auto pentRect = nk_rect(0, 0, pentImg->getWidth(), pentImg->getHeight());

        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        constexpr float pentOffset = 4.0f;
        constexpr int itemHeight = 45;
        constexpr int menuItemCount = 5;
        int y = screenH / 2.0f - (menuItemCount * itemHeight) / 2.0f;

        auto drawItem = [&](const char* text) {
            auto func = [=](nk_context* ctx, bool isActive) {
                auto textWidth = bigTGoldTextWidth(text);
                auto rect = nk_rect(screenW / 2 - textWidth / 2, y, textWidth, 45);
                nk_layout_space_push(ctx, rect);
                bigTGoldText(ctx, text, NK_TEXT_CENTERED);
                DrawFunctionResult ret = DrawFunctionResult::noAction;
                if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT))
                    ret = DrawFunctionResult::executeAction;
                rect.x -= (pentRect.w + pentOffset);
                rect.w += (pentRect.w + pentOffset) * 2;

                if (isActive)
                {
                    auto pentFrame = mBigPentagram->getCurrentFrame();
                    nk_layout_space_push(ctx, alignRect(pentRect, rect, halign_t::left, valign_t::center));
                    nk_image(ctx, pentFrame.first->getNkImage(pentFrame.second));
                    nk_layout_space_push(ctx, alignRect(pentRect, rect, halign_t::right, valign_t::center));
                    nk_image(ctx, pentFrame.first->getNkImage(pentFrame.second));
                }
                return ret;
            };
            y += itemHeight;
            return func;
        };

        FAWorld::World* world = Engine::EngineMain::get()->mWorld.get();
        mMenuItems.push_back({drawItem("Save Game"), [this, world]() {
                                  {
                                      Serial::TextWriteStream writeStream;
                                      FASaveGame::GameSaver saver(writeStream);
                                      world->save(saver);
                                      std::pair<uint8_t*, size_t> writtenData = writeStream.getData();
                                      FILE* f = fopen("save.sav", "wb");
                                      fwrite(writtenData.first, 1, writtenData.second, f);
                                      fclose(f);
                                  }
                                  mMenuHandler.engine().togglePause();
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Options"), []() { return ActionResult::continueDrawing; }});
        mMenuItems.push_back({drawItem("New Game"), []() { return ActionResult::continueDrawing; }});
        mMenuItems.push_back({drawItem("Load Game"), []() { return ActionResult::continueDrawing; }});
        mMenuItems.push_back({drawItem("Quit Diablo"), [this]() {
                                  mMenuHandler.engine().stop();
                                  return ActionResult::stopDrawing;
                              }});
    }

    void PauseMenuScreen::bigTGoldText(nk_context* ctx, const char* text, nk_flags alignment)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        nk_style_push_font(ctx, renderer->bigTGoldFont());
        nk_style_push_color(ctx, &ctx->style.text.color, nk_color{255, 255, 255, 255});
        nk_label(ctx, text, alignment);
        nk_style_pop_color(ctx);
        nk_style_pop_font(ctx);
    }

    float PauseMenuScreen::bigTGoldTextWidth(const char* text)
    {
        auto renderer = FARender::Renderer::get();
        auto fnt = renderer->bigTGoldFont();
        return fnt->width(fnt->userdata, 0.0f, text, strlen(text));
    }

    void PauseMenuScreen::menuItems(nk_context* ctx)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        nk_layout_space_begin(ctx, NK_STATIC, 0.0f, INT_MAX);
        {
            auto img = renderer->loadImage("data/diabsmal.cel");
            int32_t screenW, screenH;
            renderer->getWindowDimensions(screenW, screenH);
            constexpr int itemHeight = 45;
            constexpr int imgToMenuOffset = itemHeight;
            int menuItemCount = mMenuItems.size();
            int y = screenH / 2.0f - ((menuItemCount * itemHeight) / 2.0f) - img->getHeight() - imgToMenuOffset;
            nk_layout_space_push(ctx, nk_rect(screenW / 2 - img->getWidth() / 2, y, img->getWidth(), img->getHeight()));
            nk_image(ctx, img->getNkImage());
            if (drawMenuItems(ctx) == ActionResult::stopDrawing)
                return;
        }
        nk_layout_space_end(ctx);
    }

    void PauseMenuScreen::update(nk_context* ctx)
    {
        mBigPentagram->update();
        FARender::Renderer* renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));
        if (nk_begin(ctx, "pause menu", nk_rect(0, 0, screenW, screenH), 0))
        {
            menuItems(ctx);
            nk_end(ctx);
        }
        nk_style_pop_style_item(ctx);
    }
}
