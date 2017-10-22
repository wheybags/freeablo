#include "widgets.h"

#include <nfd.h>
#include <assert.h>
#include <cstdlib>
#include <cstring>

namespace NuklearMisc
{
    bool nk_file_pick(nk_context* ctx, const std::string& label, std::string& path, const std::string& filter, float rowHeight, float labelWidth)
    {
        // TODO: replace nasty static buffer, we could use the more advanced api, but I'm not bothered right now
        char buf[4096];

        assert(path.size()+1 < 4096);
        memcpy(buf, path.c_str(), path.size()+1);

        bool retval = false;

        nk_style_push_vec2(ctx, &ctx->style.window.group_padding, nk_vec2(0, 0));

        auto groupName = std::string("file_pick_group_") + std::to_string(size_t(&path));
        if(nk_group_begin(ctx, groupName.c_str(), NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_template_begin(ctx, rowHeight);
            {
                nk_layout_row_template_push_static(ctx, labelWidth);
                nk_layout_row_template_push_variable(ctx, 80);
                nk_layout_row_template_push_static(ctx, 40);
            }
            nk_layout_row_template_end(ctx);

            nk_label(ctx, label.c_str(), NK_TEXT_LEFT);

            if(nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, buf, 4096, nk_filter_default) == NK_EDIT_COMMITED)
                retval = true;

            if(nk_button_label(ctx, "..."))
            {
                nfdchar_t *outPath = nullptr;
                nfdresult_t result = NFD_OpenDialog(filter.c_str(), nullptr, &outPath);
                if(result == NFD_OKAY)
                {
                    path = outPath;
                    retval = true;
                }

                if(outPath != nullptr)
                    free(outPath);
            }
            else
            {
                path = buf;
            }

            nk_group_end(ctx);
        }

        nk_style_pop_vec2(ctx);
        return retval;
    }
}
