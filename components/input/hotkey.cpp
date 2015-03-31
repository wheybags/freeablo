#include "hotkey.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/python.hpp>
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

    Hotkey::Hotkey(const char *name, bpt::ptree hotkeypt)
    {
        std::string sname = name;

        key = hotkeypt.get<int>(sname + ".key");
        shift = hotkeypt.get<int>(sname + ".shift");
        ctrl = hotkeypt.get<int>(sname + ".ctrl");
        alt = hotkeypt.get<int>(sname + ".alt");
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

    void Hotkey::save(const char *name, bpt::ptree hotkeypt)
    {
        std::string sname = name;

        hotkeypt.put(sname + ".key", key);
        hotkeypt.put(sname + ".shift", int(shift));
        hotkeypt.put(sname + ".ctrl", int(ctrl));
        hotkeypt.put(sname + ".alt", int(alt));
        bpt::write_ini("resources/hotkeys.ini", hotkeypt);
    }
    
    BOOST_PYTHON_MODULE(hotkey)
    {
        boost::python::class_<Hotkey>("Hotkey")
            .def(boost::python::init<const char *, bpt::ptree>())
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
