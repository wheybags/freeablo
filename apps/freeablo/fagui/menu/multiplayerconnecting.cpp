#include "multiplayerconnecting.h"
#include "../../engine/enginemain.h"
#include "../../engine/net/client.h"
#include "../../farender/renderer.h"
#include "../menuhandler.h"
#include "startingmenuscreen.h"
#include <fa_nuklear.h>
#include <misc/misc.h>

namespace FAGui
{
    bool MultiplayerConnecting::content(nk_context* ctx)
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        if (mState == State::Connecting)
        {
            nk_label(ctx, "Connecting...", NK_TEXT_CENTERED);
        }
        else
        {
            nk_label(ctx, "Connection failed", NK_TEXT_CENTERED);
            if (nk_button_label(ctx, "Ok"))
                return true;
        }

        return false;
    }

    MultiplayerConnecting::MultiplayerConnecting(MenuHandler& menu) : MenuScreen(menu) {}

    void MultiplayerConnecting::update(nk_context* ctx)
    {
        Misc::ScopedSetter<float> setter(ctx->style.window.border, 0);
        auto renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        bool exit = false;

        if (nk_begin(
                ctx,
                "multiplayerMenu",
                nk_rect(screenW / 2 - MenuHandler::menuWidth / 2, screenH / 2 - MenuHandler::menuHeight / 2, MenuHandler::menuWidth, MenuHandler::menuHeight),
                NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
        {
            exit = content(ctx);
        }
        nk_end(ctx);

        if (exit)
            (void)mRejectAction();

        auto multiplayer = static_cast<Engine::Client*>(mMenuHandler.engine().mMultiplayer.get());

        if (multiplayer && multiplayer->didConnectionFail())
        {
            mMenuHandler.engine().mMultiplayer.reset();
            mState = State::ConnectionFailed;

            // Only now do we allow exiting this screen
            mRejectAction = [&]() {
                mMenuHandler.setActiveScreen<StartingMenuScreen>();
                return ActionResult::stopDrawing;
            };
        }
    }
}
