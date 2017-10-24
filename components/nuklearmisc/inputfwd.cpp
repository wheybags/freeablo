#include "inputfwd.h"
#include <string>
#include <cstring>

namespace NuklearMisc
{
    void handleNuklearMouseEvent(nk_context* ctx, int32_t x, int32_t y, Input::Key key, bool isDown, bool isDoubleClick)
    {
        int down = isDown;

        if (key == Input::KEY_LEFT_MOUSE)
        {
            if (isDoubleClick)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        }
        else if (key == Input::KEY_MIDDLE_MOUSE)
        {
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        }
        else if (key == Input::KEY_RIGHT_MOUSE)
        {
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        }
    }

    void handleNuklearMouseMoveEvent(nk_context* ctx, int32_t x, int32_t y, int32_t xrel, int32_t yrel)
    {
        if (ctx->input.mouse.grabbed)
        {
            int mouse_prev_x = (int)ctx->input.mouse.prev.x, mouse_prev_y = (int)ctx->input.mouse.prev.y;
            nk_input_motion(ctx, mouse_prev_x + xrel, mouse_prev_y + yrel);
        }
        else
        {
            nk_input_motion(ctx, x, y);
        }

    }

    void handleNuklearMouseWheelEvent(nk_context* ctx, int32_t x, int32_t y)
    {
        nk_input_scroll(ctx, nk_vec2((float)x, (float)y));
    }

    void handleNuklearKeyboardEvent(nk_context* ctx, bool isDown, Input::Key sym, Input::KeyboardModifiers mods)
    {
        int down = isDown;

        if (sym == Input::KEY_RSHIFT || sym == Input::KEY_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == Input::KEY_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == Input::KEY_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == Input::KEY_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == Input::KEY_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == Input::KEY_HOME) {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        }
        else if (sym == Input::KEY_END) {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        }
        else if (sym == Input::KEY_PAGEDOWN) {
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        }
        else if (sym == Input::KEY_PAGEUP) {
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        }
        else if (sym == Input::KEY_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && mods.ctrl);
        else if (sym == Input::KEY_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && mods.ctrl);
        else if (sym == Input::KEY_c)
            nk_input_key(ctx, NK_KEY_COPY, down && mods.ctrl);
        else if (sym == Input::KEY_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && mods.ctrl);
        else if (sym == Input::KEY_x)
            nk_input_key(ctx, NK_KEY_CUT, down && mods.ctrl);
        else if (sym == Input::KEY_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && mods.ctrl);
        else if (sym == Input::KEY_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && mods.ctrl);
        else if (sym == Input::KEY_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == Input::KEY_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == Input::KEY_LEFT) {
            if (mods.ctrl)
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else nk_input_key(ctx, NK_KEY_LEFT, down);
        }
        else if (sym == Input::KEY_RIGHT) {
            if (mods.ctrl)
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else nk_input_key(ctx, NK_KEY_RIGHT, down);
        }
    }

    void handleNuklearTextInputEvent(nk_context* ctx, const std::string& inp)
    {
        nk_glyph glyph;
        memcpy(glyph, inp.c_str(), NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
    }

}
