#pragma once
#include "textcolor.h"
#include <string>

namespace FAGui
{
    struct MenuEntry
    {
        std::string entry;
        FAGui::TextColor textColor = TextColor::white;
        bool clickable = true;
    };
}
