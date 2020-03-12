#include "menuscreen.h"
#include "../../engine/inputobserverinterface.h"
#include "../../farender/renderer.h"
#include "../menuhandler.h"

namespace FAGui
{
    MenuScreen::MenuScreen(MenuHandler& menu) : mMenuHandler(menu) {}
    MenuScreen::~MenuScreen() {}
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
        while (!mInputActions.empty())
        {
            auto ret = applyInputAction(mInputActions.front());
            mInputActions.pop();
            if (ret == ActionResult::stopDrawing)
                return ret;
        }

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

    MenuScreen::ActionResult MenuScreen::executeActive() { return mMenuItems[mActiveItemIndex].action(); }

    MenuScreen::ActionResult MenuScreen::applyInputAction(Engine::KeyboardInputAction action)
    {
        if (action == Engine::KeyboardInputAction::reject && mRejectAction)
            return mRejectAction();

        if (mMenuItems.empty())
            return ActionResult::stopDrawing;

        auto ret = ActionResult::continueDrawing;

        switch (action)
        {
            case Engine::KeyboardInputAction::accept:
                ret = executeActive();
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

        return ret;
    }

    void MenuScreen::notify(Engine::KeyboardInputAction action) { mInputActions.push(action); }
}
