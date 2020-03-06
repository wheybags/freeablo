#include "selectheromenuscreen.h"
#include "../../engine/enginemain.h"
#include "../../farender/animationplayer.h"
#include "../../farender/renderer.h"
#include "../../faworld/player.h"
#include "../../faworld/world.h"
#include "../menuhandler.h"
#include "../nkhelpers.h"
#include "diabloexe/characterstats.h"
#include "startingmenuscreen.h"

namespace FAGui
{
    SelectHeroMenuScreen::characterInfo::characterInfo(FAWorld::PlayerClass charClassArg, const DiabloExe::CharacterStats& stats)
    {
        level = 1;
        charClass = charClassArg;
        strength = stats.mStrength;
        magic = stats.mMagic;
        dexterity = stats.mDexterity;
        vitality = stats.mVitality;
    }

    SelectHeroMenuScreen::SelectHeroMenuScreen(MenuHandler& menu) : Parent(menu)
    {
        auto renderer = FARender::Renderer::get();
        mSmLogo = menu.createSmLogo();
        mFocus.reset(new FARender::AnimationPlayer());
        mFocus->playAnimation(renderer->loadImage("ui_art/focus.pcx&trans=0,255,0&vanim=30"),
                              FAWorld::World::getTicksInPeriod("0.06"),
                              FARender::AnimationPlayer::AnimationType::Looped);
        mFocus16.reset(new FARender::AnimationPlayer());
        mFocus16->playAnimation(renderer->loadImage("ui_art/focus16.pcx&trans=0,255,0&vanim=20"),
                                FAWorld::World::getTicksInPeriod("0.06"),
                                FARender::AnimationPlayer::AnimationType::Looped);
        setType(ContentType::chooseClass);
    }

    MenuScreen::ActionResult SelectHeroMenuScreen::chooseClass(nk_context* ctx)
    {
        nk_layout_space_push(ctx, {262, 207, 320, 33});
        menuText(ctx, "Choose Class", MenuFontColor::silver, 30, NK_TEXT_ALIGN_CENTERED);
        nk_layout_space_push(ctx, {277, 422, 140, 35});
        menuText(ctx, "OK", MenuFontColor::gold, 30, NK_TEXT_ALIGN_CENTERED);
        if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT))
            return executeActive();
        nk_layout_space_push(ctx, {427, 422, 140, 35});
        menuText(ctx, "Cancel", MenuFontColor::gold, 30, NK_TEXT_ALIGN_CENTERED);
        if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT))
            return mRejectAction();
        return drawMenuItems(ctx);
    }

    void SelectHeroMenuScreen::generateChooseClassMenu()
    {
        mRejectAction = [&]() {
            mMenuHandler.setActiveScreen<StartingMenuScreen>();
            return ActionResult::stopDrawing;
        };
        auto drawItem = [&](const char* text, const struct nk_rect& rect, FAWorld::PlayerClass type) {
            return [=](nk_context* ctx, bool isActive) {
                nk_layout_space_push(ctx, rect);
                menuText(ctx, text, MenuFontColor::gold, 24, NK_TEXT_ALIGN_CENTERED);
                DrawFunctionResult ret = DrawFunctionResult::noAction;
                if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT))
                    ret = DrawFunctionResult::setActive;
                if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_DOUBLE))
                    ret = DrawFunctionResult::executeAction;

                if (isActive)
                {
                    if (type != FAWorld::PlayerClass::none)
                    {
                        std::string classLookup;
                        switch (type)
                        {
                            case FAWorld::PlayerClass::warrior:
                                classLookup = "Warrior";
                                break;
                            case FAWorld::PlayerClass::rogue:
                                classLookup = "Rogue";
                                break;
                            case FAWorld::PlayerClass::sorceror:
                                classLookup = "Sorceror";
                                break;
                            case FAWorld::PlayerClass::none:
                                break;
                        }

                        mSelectedCharacterInfo = characterInfo{type, mMenuHandler.engine().exe().getCharacterStat(classLookup)};
                    }
                    else
                    {
                        mSelectedCharacterInfo = std::nullopt;
                    }

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
        mMenuItems.push_back({drawItem("Warrior", {262, 278, 320, 33}, FAWorld::PlayerClass::warrior), [&]() {
                                  mMenuHandler.engine().startGame(FAWorld::PlayerClass::warrior);
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Rogue", {262, 311, 320, 33}, FAWorld::PlayerClass::rogue), [&]() {
                                  mMenuHandler.engine().startGame(FAWorld::PlayerClass::rogue);
                                  return ActionResult::stopDrawing;
                              }});
        mMenuItems.push_back({drawItem("Sorceror", {262, 344, 320, 33}, FAWorld::PlayerClass::sorceror), [&]() {
                                  mMenuHandler.engine().startGame(FAWorld::PlayerClass::sorceror);
                                  return ActionResult::stopDrawing;
                              }});

        // TODO: this is hacky, we should recreate the original character select gui
        FILE* saveFile = fopen("save.sav", "rb");
        if (saveFile)
        {
            fclose(saveFile);
            mMenuItems.push_back({drawItem("Load Game", {262, 377, 320, 33}, FAWorld::PlayerClass::none), [&]() {
                                      mMenuHandler.engine().startGameFromSave("save.sav");
                                      return ActionResult::stopDrawing;
                                  }});
        }
    }

    void SelectHeroMenuScreen::setType(ContentType type)
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

    void SelectHeroMenuScreen::content(nk_context* ctx)
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

    void SelectHeroMenuScreen::update(nk_context* ctx)
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
}
