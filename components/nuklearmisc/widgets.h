#pragma once

#include <fa_nuklear.h>
#include <string>

namespace NuklearMisc
{
    bool nk_file_pick(nk_context* ctx, const std::string& label, std::string& path, const std::string& filter, float rowHeight, float labelWidth = 120);
}
