#pragma once
#include <string>

namespace Input
{
    class Hotkey
    {
    public:
        Hotkey();
        Hotkey(const char* name);
        Hotkey(const std::string& name);
        Hotkey(int nkey, bool nshift, bool nctrl, bool nalt);
        std::string name() const;
        bool operator==(const Hotkey& other);
        void save(const char* name);
        bool has_modifiers() const;

        int key;
        bool shift, ctrl, alt;
    };
}
