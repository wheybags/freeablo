#include <string>
#include <cctype>
#include <settings/settings.h>
#include "hotkey.h"
#include "keys.h"

namespace Input
{
    Hotkey::Hotkey()
    {
        key = 0;
        shift = false;
        ctrl = false;
        alt = false;
    }

    Hotkey::Hotkey(const char *name) : Hotkey(std::string(name)) {}

    Hotkey::Hotkey(const std::string& name)
    {
        Settings::Settings hotkeySettings;
        hotkeySettings.loadFromFile("resources/hotkeys.ini");

        key = hotkeySettings.get<int>(name, "key");
        shift = hotkeySettings.get<int>(name, "shift");
        ctrl = hotkeySettings.get<int>(name, "ctrl");
        alt = hotkeySettings.get<int>(name, "alt");
    }
    
    Hotkey::Hotkey(int nkey, bool nshift, bool nctrl, bool nalt)
    {
        key = nkey;
        shift = nshift;
        ctrl = nctrl;
        alt = nalt;
    }

    std::string Hotkey::name() const
    {
        std::string hotkeyName;
        if (shift)
        {
            hotkeyName += "SHIFT + ";
        }
        if (ctrl)
        {
            hotkeyName += "CTRL + ";
        }
        if (alt)
        {
            hotkeyName += "ALT + ";
        }

        switch(key)
        {
            case Input::KEY_F1: hotkeyName += "F1"; break;
            case Input::KEY_F2: hotkeyName += "F2"; break;
            case Input::KEY_F3: hotkeyName += "F3"; break;
            case Input::KEY_F4: hotkeyName += "F4"; break;
            case Input::KEY_F5: hotkeyName += "F5"; break;
            case Input::KEY_F6: hotkeyName += "F6"; break;
            case Input::KEY_F7: hotkeyName += "F7"; break;
            case Input::KEY_F8: hotkeyName += "F8"; break;
            case Input::KEY_F9: hotkeyName += "F9"; break;
            case Input::KEY_F10: hotkeyName += "F10"; break;
            case Input::KEY_F11: hotkeyName += "F11"; break;
            case Input::KEY_F12: hotkeyName += "F12"; break;
            case Input::KEY_F13: hotkeyName += "F13"; break;
            case Input::KEY_F14: hotkeyName += "F14"; break;
            case Input::KEY_F15: hotkeyName += "F15"; break;

            case Input::KEY_BACKSPACE: hotkeyName += "BACKSPACE"; break;
            case Input::KEY_TAB: hotkeyName += "TAB"; break;
            case Input::KEY_CLEAR: hotkeyName += "CLEAR"; break;
            case Input::KEY_SPACE: hotkeyName += "SPACE"; break;
            case Input::KEY_DELETE: hotkeyName += "DELETE"; break;

            case Input::KEY_KP0: hotkeyName += "KP0"; break;
            case Input::KEY_KP1: hotkeyName += "KP1"; break;
            case Input::KEY_KP2: hotkeyName += "KP2"; break;
            case Input::KEY_KP3: hotkeyName += "KP3"; break;
            case Input::KEY_KP4: hotkeyName += "KP4"; break;
            case Input::KEY_KP5: hotkeyName += "KP5"; break;
            case Input::KEY_KP6: hotkeyName += "KP6"; break;
            case Input::KEY_KP7: hotkeyName += "KP7"; break;
            case Input::KEY_KP8: hotkeyName += "KP8"; break;
            case Input::KEY_KP9: hotkeyName += "KP9"; break;
            case Input::KEY_KP_PERIOD: hotkeyName += "KP_PERIOD"; break;
            case Input::KEY_KP_DIVIDE: hotkeyName += "KP_DIVIDE"; break;
            case Input::KEY_KP_MULTIPLY: hotkeyName += "KP_MULTIPLY"; break;
            case Input::KEY_KP_MINUS: hotkeyName += "KP_MINUS"; break;
            case Input::KEY_KP_PLUS: hotkeyName += "KP_PLUS"; break;
            case Input::KEY_KP_EQUALS: hotkeyName += "KP_EQUALS"; break;

            case Input::KEY_UP: hotkeyName += "UP"; break;
            case Input::KEY_DOWN: hotkeyName += "DOWN"; break;
            case Input::KEY_RIGHT: hotkeyName += "RIGHT"; break;
            case Input::KEY_LEFT: hotkeyName += "LEFT"; break;
            case Input::KEY_INSERT: hotkeyName += "INSERT"; break;
            case Input::KEY_HOME: hotkeyName += "HOME"; break;
            case Input::KEY_END: hotkeyName += "END"; break;
            case Input::KEY_PAGEUP: hotkeyName += "PAGEUP"; break;
            case Input::KEY_PAGEDOWN: hotkeyName += "PAGEDOWN"; break;

            default:
            {
               hotkeyName += std::toupper(char(key));
            }
        }

        return hotkeyName;
    }

    bool Hotkey::operator==(const Hotkey &other)
    {
         if (key == other.key && shift == other.shift && ctrl == other.ctrl && alt == other.alt)
         {
             return true;
         }
         else
         {
             return false;
         }
    }

    void Hotkey::save(const char *name)
    {
        std::string sname = name;

        Settings::Settings hotkeySettings;
        hotkeySettings.loadFromFile("resources/hotkeys.ini");

        hotkeySettings.set<int>(sname, "key", key);
        hotkeySettings.set<int>(sname, "shift", int(shift));
        hotkeySettings.set<int>(sname, "ctrl", int(ctrl));
        hotkeySettings.set<int>(sname, "alt", int(alt));
        hotkeySettings.save();
    }
}
