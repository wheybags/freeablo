#include "menuhandler.h"
#include "../engine/enginemain.h"
#include "../farender/animationplayer.h"
#include "../farender/renderer.h"
#include "../fasavegame/gameloader.h"
#include "boost/make_unique.hpp"
#include "diabloexe/characterstats.h"
#include "fa_nuklear.h"
#include "guimanager.h"
#include "nkhelpers.h"
#include "serial/textstream.h"

namespace FAGui
{
    MenuScreen::MenuScreen(MenuHandler& menu) : mMenuHandler(menu) {}

    MenuScreen::~MenuScreen() {}

    PauseMenuScreen::PauseMenuScreen(MenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mBigPentagram.reset(new FARender::AnimationPlayer());
        auto pentImg = renderer->loadImage("data/pentspin.cel");
        mBigPentagram->playAnimation(pentImg, FAWorld::World::getTicksInPeriod(0.06f), FARender::AnimationPlayer::AnimationType::Looped);
        auto pentRect = nk_rect(0, 0, pentImg->getWidth(), pentImg->getHeight());

        int y = 115;
        constexpr float pentOffset = 4.0f;
        constexpr int itemHeight = 45;
        auto drawItem = [&](const char* text) {
            auto func = [=](nk_context* ctx, bool isActive) {
                auto textWidth = bigTGoldTextWidth(text);
                int32_t screenW, screenH;
                renderer->getWindowDimensions(screenW, screenH);
                auto rect = nk_rect(screenW / 2 - textWidth / 2, y, textWidth, 45);
                nk_layout_space_push(ctx, rect);
                bigTGoldText(ctx, text, NK_TEXT_CENTERED);
                DrawFunctionResult ret = DrawFunctionResult::noAction;
                if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
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
        mMenuItems.push_back({drawItem("Save Game"), [this]() {
                                  {
                                      Serial::TextWriteStream writeStream;
                                      FASaveGame::GameSaver saver(writeStream);
                                      FAWorld::World::get()->save(saver);
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
                                  mMenuHandler.quit();
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
            nk_layout_space_push(ctx, nk_rect(screenW / 2 - img->getWidth() / 2, 0, img->getWidth(), img->getHeight()));
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

    StartingScreen::StartingScreen(MenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mFocus42.reset(new FARender::AnimationPlayer());
        mFocus42->playAnimation(renderer->loadImage("ui_art/focus42.pcx&trans=0,255,0&vanim=42"),
                                FAWorld::World::getTicksInPeriod(0.06f),
                                FARender::AnimationPlayer::AnimationType::Looped);
        mSmLogo = menu.createSmLogo();

        auto drawItem = [&](const char* text, const struct nk_rect& rect) {
            return [=](nk_context* ctx, bool isActive) {
                nk_layout_space_push(ctx, rect);
                menuText(ctx, text, MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                auto ret = DrawFunctionResult::noAction;
                if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
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
                                  mMenuHandler.setActiveScreen<SelectHeroScreen>();
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Multi Player", {65, 235, 510, 42}), [this]() { return ActionResult::continueDrawing; }});
        mMenuItems.push_back({drawItem("Replay Intro", {65, 277, 510, 42}), [this]() { return ActionResult::continueDrawing; }});
        mMenuItems.push_back({drawItem("Show Credits", {65, 320, 510, 42}), [this]() { return ActionResult::continueDrawing; }});
        mRejectAction = [this]() {
            mMenuHandler.engine().stop();
            return ActionResult::stopDrawing;
        };
        mMenuItems.push_back({drawItem("Exit Diablo", {65, 363, 510, 42}), mRejectAction});
    }

    void StartingScreen::menuItems(nk_context* ctx)
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

    void StartingScreen::update(nk_context* ctx)
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

    SelectHeroScreen::characterInfo::characterInfo(ClassType charClassArg, const DiabloExe::CharacterStats& stats)
    {
        level = 1;
        charClass = charClassArg;
        strength = stats.mStrength;
        magic = stats.mMagic;
        dexterity = stats.mDexterity;
        vitality = stats.mVitality;
    }

    SelectHeroScreen::SelectHeroScreen(MenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mSmLogo = menu.createSmLogo();
        mFocus.reset(new FARender::AnimationPlayer());
        mFocus->playAnimation(renderer->loadImage("ui_art/focus.pcx&trans=0,255,0&vanim=30"),
                              FAWorld::World::getTicksInPeriod(0.06f),
                              FARender::AnimationPlayer::AnimationType::Looped);
        mFocus16.reset(new FARender::AnimationPlayer());
        mFocus16->playAnimation(renderer->loadImage("ui_art/focus16.pcx&trans=0,255,0&vanim=20"),
                                FAWorld::World::getTicksInPeriod(0.06f),
                                FARender::AnimationPlayer::AnimationType::Looped);
        setType (ContentType::chooseClass);
    }

    MenuScreen::ActionResult SelectHeroScreen::chooseClass(nk_context* ctx)
    {
        nk_layout_space_push(ctx, {262, 207, 320, 33});
        menuText(ctx, "Choose Class", MenuFontColor::silver, 30, NK_TEXT_ALIGN_CENTERED);
        bool executeCurrent = false;
        nk_layout_space_push(ctx, {277, 422, 140, 35});
        menuText(ctx, "OK", MenuFontColor::gold, 30, NK_TEXT_ALIGN_CENTERED);
        if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
            executeCurrent = true;
        nk_layout_space_push(ctx, {427, 422, 140, 35});
        menuText(ctx, "Cancel", MenuFontColor::gold, 30, NK_TEXT_ALIGN_CENTERED);
        if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
            return mRejectAction();
        return drawMenuItems(ctx);
    }

    void SelectHeroScreen::generateChooseClassMenu()
    {
        mRejectAction = [&]() {
            mMenuHandler.setActiveScreen<StartingScreen>();
            return ActionResult::stopDrawing;
        };
        auto drawItem = [&](const char* text, const struct nk_rect& rect, ClassType type) {
            return [=](nk_context* ctx, bool isActive) {
                nk_layout_space_push(ctx, rect);
                menuText(ctx, text, MenuFontColor::gold, 24, NK_TEXT_ALIGN_CENTERED);
                DrawFunctionResult ret = DrawFunctionResult::noAction;
                if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
                    ret = DrawFunctionResult::setActive;
                if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_DOUBLE, true))
                    ret = DrawFunctionResult::executeAction;
                if (isActive)
                {
                    mSelectedCharacterInfo = characterInfo{type, mMenuHandler.engine().exe().getCharacterStat(text)};
                    auto frame = mFocus->getCurrentFrame();
                    auto frameRect = nk_rect(0, 0, frame.first->getWidth(), frame.first->getHeight());
                    nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::left, valign_t::center));
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                    nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::right, valign_t::center));
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                }
                return ret;
            };
        };
        mMenuItems.clear();
        mMenuItems.push_back({drawItem("Warrior", {262, 278, 320, 33}, ClassType::warrior), [&]() {
                                  mMenuHandler.engine().startGame("Warrior");
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Rogue", {262, 311, 320, 33}, ClassType::rogue), [&]() {
                                  mMenuHandler.engine().startGame("Rogue");
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Sorcerer", {262, 344, 320, 33}, ClassType::sorcerer), [&]() {
                                  mMenuHandler.engine().startGame("Sorcerer");
                                  return ActionResult::stopDrawing;
                              }});
    }

    void SelectHeroScreen::setType(ContentType type)
    {
        mContentType = type;
        switch (type)
        {
            case ContentType::heroList:
                break;
            case ContentType::chooseClass:
                generateChooseClassMenu();
                break;
            case ContentType::enterName:
                break;
        }
    }

    void SelectHeroScreen::content(nk_context* ctx)
    {
        nk_layout_space_begin(ctx, NK_STATIC, 48, INT_MAX);
        // NOTE: similar to starting screen position, reuse
        nk_layout_space_push(ctx, {125, 0, 390, 154});
        {
            auto frame = mSmLogo->getCurrentFrame();
            nk_image(ctx, frame.first->getNkImage(frame.second));
        }
        nk_layout_space_push(ctx, {25, 161, 590, 35});
        {
            menuText(ctx,
                     [&]() {
                         switch (mContentType)
                         {
                             case ContentType::heroList:
                                 return "Single Player Characters";
                             case ContentType::chooseClass:
                             case ContentType::enterName:
                                 return "New Single Player Hero";
                         }
                         return "";
                     }(),
                     MenuFontColor::silver,
                     30,
                     NK_TEXT_ALIGN_CENTERED);
        }
        auto draw_param_value = [&](const std::string& param, const std::string& value, float y) {
            nk_layout_space_push(ctx, {34, y, 110, 21});
            menuText(ctx, param.c_str(), MenuFontColor::silver, 16, NK_TEXT_ALIGN_RIGHT);
            nk_layout_space_push(ctx, {154, y, 40, 21});
            menuText(ctx, value.c_str(), MenuFontColor::silver, 16, NK_TEXT_CENTERED);
        };
        auto to_string = [&](int(characterInfo::*member)) { return mSelectedCharacterInfo ? std::to_string((*mSelectedCharacterInfo).*member) : "--"; };
        draw_param_value("Level:", to_string(&characterInfo::level), 315);
        draw_param_value("Strength:", to_string(&characterInfo::strength), 351);
        draw_param_value("Magic:", to_string(&characterInfo::magic), 372);
        draw_param_value("Dexterity:", to_string(&characterInfo::dexterity), 393);
        draw_param_value("Vitality:", to_string(&characterInfo::vitality), 414);
        switch (mContentType)
        {
            case ContentType::heroList:
                break;
            case ContentType::chooseClass:
                if (chooseClass(ctx) == ActionResult::stopDrawing)
                    return;
            case ContentType::enterName:
                break;
        }

        nk_layout_space_push(ctx, {26, 207, 180, 76});
        {
            auto renderer = FARender::Renderer::get();
            auto heros_img =
                renderer->loadImage("ui_art/heros.pcx&vanim=76")->getNkImage(mSelectedCharacterInfo ? static_cast<int>(mSelectedCharacterInfo->charClass) : 3);
            nk_image(ctx, heros_img);
        }
        nk_layout_space_end(ctx);
    }

    void SelectHeroScreen::update(nk_context* ctx)
    {
        for (auto ptr : {mSmLogo.get(), mFocus.get(), mFocus16.get()})
            ptr->update();
        Misc::ScopedSetter<float> setter(ctx->style.window.border, 0);
        auto renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        auto bg = renderer->loadImage("ui_art/selhero.pcx")->getNkImage();
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(bg));
        if (nk_begin(
                ctx,
                "selectHeroScreen",
                nk_rect(screenW / 2 - MenuHandler::menuWidth / 2, screenH / 2 - MenuHandler::menuHeight / 2, MenuHandler::menuWidth, MenuHandler::menuHeight),
                NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
        {
            content(ctx);
        }
        nk_end(ctx);
        nk_style_pop_style_item(ctx);
    }

    std::unique_ptr<FARender::AnimationPlayer> MenuHandler::createSmLogo()
    {
        auto ret = boost::make_unique<FARender::AnimationPlayer>();
        auto renderer = FARender::Renderer::get();
        ret->playAnimation(renderer->loadImage("ui_art/smlogo.pcx&trans=0,255,0&vanim=154"),
                           FAWorld::World::getTicksInPeriod(0.06f),
                           FARender::AnimationPlayer::AnimationType::Looped);
        return ret;
    }

    MenuHandler::MenuHandler(Engine::EngineMain& engine) : mEngine(engine) {}

    void MenuHandler::update(nk_context* ctx) const
    {
        if (mActiveScreen)
            mActiveScreen->update(ctx);
    }

    void MenuHandler::quit() { mEngine.stop(); }

    void MenuHandler::disable() { mActiveScreen.reset(); }

    void MenuHandler::notify(Engine::KeyboardInputAction action)
    {
        if (mActiveScreen)
            mActiveScreen->notify(action);
    }

    void MenuScreen::notify(Engine::KeyboardInputAction action)
    {
        if (mMenuItems.empty())
            return;
        switch (action)
        {
            case Engine::KeyboardInputAction::accept:
                mMenuItems[mActiveItemIndex].action();
                return;
            case Engine::KeyboardInputAction::reject:
                if (mRejectAction)
                    mRejectAction();
                break;
            case Engine::KeyboardInputAction::nextOption:
                mActiveItemIndex = (mActiveItemIndex + 1) % mMenuItems.size();
                break;
            case Engine::KeyboardInputAction::prevOption:
                mActiveItemIndex = (mActiveItemIndex - 1 + mMenuItems.size()) % mMenuItems.size();
                break;
            default:
                break;
        }
    }

    void MenuScreen::menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        nk_style_push_color(ctx, &ctx->style.text.color, nk_color{255, 255, 255, 255});
        switch (color)
        {
            case MenuFontColor::gold:
                nk_style_push_font(ctx, renderer->goldFont(fontSize));
                break;
            case MenuFontColor::silver:
                nk_style_push_font(ctx, renderer->silverFont(fontSize));
                break;
        }
        nk_label(ctx, text, textAlignment);
        nk_style_pop_color(ctx);
        nk_style_pop_font(ctx);
    }

    MenuScreen::ActionResult MenuScreen::drawMenuItems(nk_context* ctx)
    {
        int index = 0;
        for (auto& item : mMenuItems)
        {
            switch (item.drawFunction(ctx, mActiveItemIndex == index))
            {
                case DrawFunctionResult::executeAction:
                {
                    mActiveItemIndex = index;
                    auto ret = item.action();
                    switch (ret)
                    {
                        case ActionResult::stopDrawing:
                            return ret;
                        case ActionResult::continueDrawing:
                            break;
                    }
                    break;
                }
                case DrawFunctionResult::setActive:
                    mActiveItemIndex = index;
                    break;
                case DrawFunctionResult::noAction:
                    break;
            }
            ++index;
        }

        return ActionResult::continueDrawing;
    }
}
