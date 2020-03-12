#pragma once
struct nk_color;

namespace FAGui
{
    enum class TextColor
    {
        white,
        blue,
        golden,
        red,
    };

    nk_color getNkColor(TextColor color);
}
