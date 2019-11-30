#pragma once
#include "textcolor.h"
#include <string>

namespace FAGui
{
    struct MenuEntry
    {
        std::string entry;
        FAGui::TextColor textColor;
        bool clickable;

        MenuEntry(const std::string& e = "", FAGui::TextColor tc = FAGui::TextColor::white, bool cl = true) : entry(e), textColor(tc), clickable(cl) {}
    };
}
