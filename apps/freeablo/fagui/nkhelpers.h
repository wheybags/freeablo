#pragma once
#include "nuklearmisc/inputfwd.h"

// file to host common functions useful for working with nuklear library

namespace FAGui
{
    enum struct halign_t
    {
        left,
        center,
        right,
    };
    enum struct valign_t
    {
        top,
        center,
        bottom,
    };

    // returns center of nk_rect as nk_vec2
    struct nk_vec2 center(const struct nk_rect& rect);
    // performs nk_widget_has_mouse_click_down and checks that mouse button is down
    bool nk_widget_is_mouse_click_down_inactive(nk_context* ctx, nk_buttons buttons);
    // nk_widget_hovered without regard to activity of window
    bool nk_inactive_widget_is_hovered(nk_context* ctx);
    // checks if cursor has left the widget and hover effect should become inactive
    bool nk_widget_mouse_left(nk_context* ctx);
    // aligns inner_rect relatively to outer_rect using halign, valign as guides for positioning
    struct nk_rect alignRect(const struct nk_rect& inner_rect, const struct nk_rect& outer_rect, halign_t halign, valign_t valign);
}
