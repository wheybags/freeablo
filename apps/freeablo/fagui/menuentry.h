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

        MenuEntry() : entry(""), textColor(TextColor::white), clickable(false) {}
        MenuEntry(const std::string& e) : entry(e), textColor(TextColor::white), clickable(true) {}
        MenuEntry(const std::string& e, TextColor tc) : entry(e), textColor(tc), clickable(true) {}
        MenuEntry(const std::string& e, bool cl) : entry(e), textColor(TextColor::white), clickable(cl) {}
        MenuEntry(const std::string& e, TextColor tc, bool cl) : entry(e), textColor(tc), clickable(cl) {}
    };
}
