#include "nkhelpers.h"
#include "fa_nuklear.h"

struct nk_vec2 FAGui::center(const struct nk_rect& rect) { return {rect.x + rect.w / 2, rect.y + rect.h / 2}; }

bool FAGui::nk_widget_is_mouse_click_down_inactive(nk_context* ctx, nk_buttons buttons)
{
    return nk_inactive_widget_is_hovered(ctx) && nk_input_is_mouse_click_down_in_rect(&ctx->input, buttons, nk_widget_bounds(ctx), true);
}

bool FAGui::nk_inactive_widget_is_hovered(nk_context* ctx) { return nk_input_is_mouse_hovering_rect(&ctx->input, nk_widget_bounds(ctx)); }

bool FAGui::nk_widget_mouse_left(nk_context* ctx)
{
    return !nk_inactive_widget_is_hovered(ctx) && nk_input_is_mouse_prev_hovering_rect(&ctx->input, nk_widget_bounds(ctx));
}

struct nk_rect FAGui::alignRect(const struct nk_rect& inner_rect, const struct nk_rect& outer_rect, halign_t halign, valign_t valign)
{
    auto c = center(outer_rect);
    auto shift = (outer_rect.w - inner_rect.w) / 2;
    switch (halign)
    {
        case halign_t::left:
            c.x -= shift;
            break;
        case halign_t::right:
            c.x += shift;
            break;
        default:
            break;
    }
    shift = (outer_rect.h - inner_rect.h) / 2;
    switch (valign)
    {
        case valign_t::top:
            c.y -= shift;
            break;
        case valign_t::bottom:
            c.y += shift;
            break;
        default:
            break;
    }
    return {c.x - inner_rect.w / 2, c.y - inner_rect.h / 2, inner_rect.w, inner_rect.h};
}
