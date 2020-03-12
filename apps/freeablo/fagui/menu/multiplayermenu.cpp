#include "multiplayermenu.h"
#include "../../engine/enginemain.h"
#include "../../farender/renderer.h"
#include "../menuhandler.h"
#include "multiplayerconnecting.h"
#include "startingmenuscreen.h"
#include <climits>
#include <cstring>
#include <fa_nuklear.h>
#include <misc/misc.h>

namespace FAGui
{
    bool MultiplayerMenu::content(nk_context* ctx)
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, "Enter Server IP / Hostname", NK_TEXT_CENTERED);

        nk_edit_focus(ctx, 0);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, mIpBuffer, IP_BUFFER_SIZE, nk_filter_default);
        if (nk_button_label(ctx, "Connect"))
        {
            mMenuHandler.engine().mSettings.set("multiplayer", "last_server", std::string(mIpBuffer));
            mMenuHandler.engine().mSettings.save();
            return true;
        }

        return false;
    }

    MultiplayerMenu::MultiplayerMenu(MenuHandler& menu) : MenuScreen(menu)
    {
        mRejectAction = [&]() {
            mMenuHandler.setActiveScreen<StartingMenuScreen>();
            return ActionResult::stopDrawing;
        };

        std::string lastServer = mMenuHandler.engine().mSettings.get<std::string>("multiplayer", "last_server", "");
        if (!lastServer.empty() && lastServer.size() < IP_BUFFER_SIZE)
            memcpy(mIpBuffer, lastServer.c_str(), lastServer.size() + 1);
    }

    void MultiplayerMenu::update(nk_context* ctx)
    {
        Misc::ScopedSetter<float> setter(ctx->style.window.border, 0);
        auto renderer = FARender::Renderer::get();
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        bool doConnect = false;

        if (nk_begin(
                ctx,
                "multiplayerMenu",
                nk_rect(screenW / 2 - MenuHandler::menuWidth / 2, screenH / 2 - MenuHandler::menuHeight / 2, MenuHandler::menuWidth, MenuHandler::menuHeight),
                NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND))
        {
            doConnect = content(ctx);
        }
        nk_end(ctx);

        if (doConnect)
        {
            mMenuHandler.engine().startMultiplayerGame(mIpBuffer);
            mMenuHandler.setActiveScreen<MultiplayerConnecting>();
        }
    }
}
