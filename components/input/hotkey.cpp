#include "hotkey.h"
#include <settings/settings.h>
#include <misc/boost_python.h>
#include <string>


namespace Input
{
    Hotkey::Hotkey()
    {
        key = 0;
        shift = false;
        ctrl = false;
        alt = false;
    }

    Hotkey::Hotkey(const char *name)
    {
        std::string sname = name;

        Settings::Settings hotkeySettings;
        hotkeySettings.loadFromFile("resources/hotkeys.ini");

        key = hotkeySettings.get<int>(sname, "key");
        shift = hotkeySettings.get<int>(sname, "shift");
        ctrl = hotkeySettings.get<int>(sname, "ctrl");
        alt = hotkeySettings.get<int>(sname, "alt");
    }
    
    Hotkey::Hotkey(int nkey, bool nshift, bool nctrl, bool nalt)
    {
        key = nkey;
        shift = nshift;
        ctrl = nctrl;
        alt = nalt;
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
    
    BOOST_PYTHON_MODULE(hotkey)
    {
        boost::python::class_<Hotkey>("Hotkey")
            .def(boost::python::init<const char *>())
            .def(boost::python::init<int, bool, bool, bool>())
            .def("__eq__", &Hotkey::operator==)
            .def("save", &Hotkey::save)
            .def_readwrite("key", &Hotkey::key)
            .def_readwrite("shift", &Hotkey::shift)
            .def_readwrite("ctrl", &Hotkey::ctrl)
            .def_readwrite("alt", &Hotkey::alt);
    }
    
    void Hotkey::initpythonwrapper()
    {
        inithotkey();
    }
    
}
