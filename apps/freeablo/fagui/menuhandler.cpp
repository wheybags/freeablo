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
        mBigPentagram->playAnimation(
            renderer->loadImage("data/pentspin.cel"), FAWorld::World::getTicksInPeriod(0.06f), FARender::AnimationPlayer::AnimationType::Looped);
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
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        nk_layout_space_begin(ctx, NK_STATIC, 0.0f, INT_MAX);
        {
            auto img = renderer->loadImage("data/diabsmal.cel");
            nk_layout_space_push(ctx, nk_rect(screenW / 2 - img->getWidth() / 2, 0, img->getWidth(), img->getHeight()));
            nk_image(ctx, img->getNkImage());
            int y = 115;
            int itemIndex = 0;
            constexpr int itemHeight = 45;
            auto pentFrame = mBigPentagram->getCurrentFrame();
            auto pentRect = nk_rect(0, 0, pentFrame.first->getWidth(), pentFrame.first->getHeight());
            constexpr float pentOffset = 4.0f;
            auto addItem = [&](const char* text, std::function<bool()> action) {
                auto textWidth = bigTGoldTextWidth(text);
                auto rect = nk_rect(screenW / 2 - textWidth / 2, y, textWidth, 45);
                nk_layout_space_push(ctx, rect);
                bigTGoldText(ctx, text, NK_TEXT_CENTERED);
                if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
                {
                    activeItemIndex = itemIndex;
                    if (action())
                        return true;
                }
                rect.x -= (pentRect.w + pentOffset);
                rect.w += (pentRect.w + pentOffset) * 2;

                if (activeItemIndex == itemIndex)
                {
                    if (nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER))
                    {
                        if (action())
                            return true;
                    }
                    nk_layout_space_push(ctx, alignRect(pentRect, rect, halign_t::left, valign_t::center));
                    nk_image(ctx, pentFrame.first->getNkImage(pentFrame.second));
                    nk_layout_space_push(ctx, alignRect(pentRect, rect, halign_t::right, valign_t::center));
                    nk_image(ctx, pentFrame.first->getNkImage(pentFrame.second));
                }

                y += itemHeight;
                ++itemIndex;
                return false;
            };
            if (addItem("Save Game", [this]() {
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
                    mMenuHandler.engine().togglePause();
                    return true;
                }))
                return;
            if (addItem("Options", []() { return false; }))
                return;
            if (addItem("New Game", []() { return false; }))
                return;
            if (addItem("Load Game", []() { return false; }))
                return;
            if (addItem("Quit Diablo", [this]() {
                    mMenuHandler.quit();
                    return true;
                }))
                return;
            // TODO: support autorepeat
            if (nk_input_is_key_pressed(&ctx->input, NK_KEY_UP))
                --activeItemIndex;
            if (nk_input_is_key_pressed(&ctx->input, NK_KEY_DOWN))
                ++activeItemIndex;
            activeItemIndex = (activeItemIndex + itemIndex) % itemIndex;
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

            int itemIndex = 0;
            auto addItem = [&](const char* text, const struct nk_rect& rect, std::function<bool()> action) {
                nk_layout_space_push(ctx, rect);
                menuText(ctx, text, MenuFontColor::gold, 42, NK_TEXT_ALIGN_CENTERED);
                if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
                {
                    activeItemIndex = itemIndex;
                    if (action())
                        return true;
                }
                if (activeItemIndex == itemIndex)
                {
                    if (nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER))
                    {
                        if (action())
                            return true;
                    }
                    auto frame = mFocus42->getCurrentFrame();
                    auto frameRect = nk_rect(0, 0, frame.first->getWidth(), frame.first->getHeight());
                    nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::left, valign_t::center));
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                    nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::right, valign_t::center));
                    nk_image(ctx, frame.first->getNkImage(frame.second));
                }
                ++itemIndex;
                return false;
            };
            if (addItem("Single Player", {65, 192, 510, 42}, [this]() {
                    mMenuHandler.setActiveScreen<SelectHeroScreen>();
                    return true;
                }))
                return;
            if (addItem("Multi Player", {65, 235, 510, 42}, []() { return false; }))
                return;
            if (addItem("Replay Intro", {65, 277, 510, 42}, []() { return false; }))
                return;
            if (addItem("Show Credits", {65, 320, 510, 42}, []() { return false; }))
                return;
            if (addItem("Exit Diablo", {65, 363, 510, 42}, [this]() {
                    mMenuHandler.quit();
                    return true;
                }))
                return;
            if (nk_input_is_key_pressed(&ctx->input, NK_KEY_UP))
                --activeItemIndex;
            if (nk_input_is_key_pressed(&ctx->input, NK_KEY_DOWN))
                ++activeItemIndex;
            activeItemIndex = (activeItemIndex + itemIndex) % itemIndex;

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
    }

    bool SelectHeroScreen::chooseClass(nk_context* ctx)
    {
        nk_layout_space_push(ctx, {262, 207, 320, 33});
        menuText(ctx, "Choose Class", MenuFontColor::silver, 30, NK_TEXT_ALIGN_CENTERED);
        int itemIndex = 0;
        bool executeCurrent = false;
        nk_layout_space_push(ctx, {277, 422, 140, 35});
        menuText(ctx, "OK", MenuFontColor::gold, 30, NK_TEXT_ALIGN_CENTERED);
        if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
            executeCurrent = true;
        nk_layout_space_push(ctx, {427, 422, 140, 35});
        menuText(ctx, "Cancel", MenuFontColor::gold, 30, NK_TEXT_ALIGN_CENTERED);
        auto exitAction = [&]() {
            mMenuHandler.setActiveScreen<StartingScreen>();
            return true;
        };

        if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
            return exitAction();
        auto addItem = [&](const char* text, const struct nk_rect& rect, std::function<bool()> action) {
            nk_layout_space_push(ctx, rect);
            menuText(ctx, text, MenuFontColor::gold, 24, NK_TEXT_ALIGN_CENTERED);
            if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_LEFT, true))
            {
                activeItemIndex = itemIndex;
            }
            if (nk_widget_is_mouse_click_down(ctx, NK_BUTTON_DOUBLE, true))
            {
                activeItemIndex = itemIndex;
                if (action())
                    return true;
            }
            if (activeItemIndex == itemIndex)
            {
                mSelectedCharacterInfo = characterInfo{static_cast<ClassType>(itemIndex), mMenuHandler.engine().exe().getCharacterStat(text)};
                if (executeCurrent || nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER))
                {
                    if (action())
                        return true;
                }
                auto frame = mFocus->getCurrentFrame();
                auto frameRect = nk_rect(0, 0, frame.first->getWidth(), frame.first->getHeight());
                nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::left, valign_t::center));
                nk_image(ctx, frame.first->getNkImage(frame.second));
                nk_layout_space_push(ctx, alignRect(frameRect, rect, halign_t::right, valign_t::center));
                nk_image(ctx, frame.first->getNkImage(frame.second));
            }
            ++itemIndex;
            return false;
        };
        if (addItem("Warrior", {262, 278, 320, 33}, [&]() {
                mMenuHandler.engine().startGame("Warrior");
                return true;
            }))
            return true;
        if (addItem("Rogue", {262, 311, 320, 33}, [&]() {
                mMenuHandler.engine().startGame("Rogue");
                return true;
            }))
            return true;
        if (addItem("Sorcerer", {262, 344, 320, 33}, [&]() {
                mMenuHandler.engine().startGame("Sorcerer");
                return true;
            }))
            return true;
        nk_layout_space_push(ctx, {280, 425, 140, 35});

        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_TEXT_RESET_MODE))
            return exitAction();
        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_UP))
            --activeItemIndex;
        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_DOWN))
            ++activeItemIndex;
        activeItemIndex = (activeItemIndex + itemIndex) % itemIndex;
        return false;
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
                if (chooseClass(ctx))
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
}
