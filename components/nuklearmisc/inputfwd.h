#pragma once

#include <fa_nuklear.h>

#include <input/inputmanager.h>

namespace NuklearMisc
{
    void handleNuklearMouseEvent(nk_context* ctx, int32_t x, int32_t y, Input::Key key, bool isDown, bool isDoubleClick);
    void handleNuklearMouseMoveEvent(nk_context* ctx, int32_t x, int32_t y, int32_t xrel, int32_t yrel);
    void handleNuklearMouseWheelEvent(nk_context* ctx, int32_t x, int32_t y);
    void handleNuklearKeyboardEvent(nk_context* ctx, bool isDown, Input::Key sym, Input::KeyboardModifiers mods);
    void handleNuklearTextInputEvent(nk_context* ctx, const std::string& inp);
}
