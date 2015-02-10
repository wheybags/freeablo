#include "hotkey.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/python.hpp>
#include <string>


namespace Input
{
    Hotkey::Hotkey(){}

    Hotkey::Hotkey(const char *name, bpt::ptree hotkeypt)
    {
        std::string sname = name;

        key = hotkeypt.get<int>(sname + ".key");
        shift = hotkeypt.get<int>(sname + ".shift");
        ctrl = hotkeypt.get<int>(sname + ".ctrl");
        alt = hotkeypt.get<int>(sname + ".alt");
        super = hotkeypt.get<int>(sname + ".super");
    }
    
    Hotkey::Hotkey(int nkey, bool nshift, bool nctrl, bool nalt, bool nsuper)
    {
        key = nkey;
        shift = nshift;
        ctrl = nctrl;
        alt = nalt;
        super = nsuper;
    }

    bool Hotkey::operator==(const Hotkey &other)
    {
         if (key == other.key && shift == other.shift && ctrl == other.ctrl && alt == other.alt && super == other.super)
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
        hotkeypt.put(sname + ".shift", shift);
        hotkeypt.put(sname + ".ctrl", ctrl);
        hotkeypt.put(sname + ".alt", alt);
        hotkeypt.put(sname + ".super", super);
        bpt::write_ini("resources/hotkeys.ini", hotkeypt);
    }
    
    BOOST_PYTHON_MODULE(hotkey)
    {
        boost::python::class_<Hotkey>("Hotkey")
            .def(boost::python::init<const char *, bpt::ptree>())
            .def(boost::python::init<int, bool, bool, bool, bool>())
            .def(boost::python::self == boost::python::self)
            .def("save", &Hotkey::save)
            .def_readwrite("key", &Hotkey::key)
            .def_readwrite("shift", &Hotkey::shift)
            .def_readwrite("ctrl", &Hotkey::ctrl)
            .def_readwrite("alt", &Hotkey::alt)
            .def_readwrite("super", &Hotkey::super);
    }
    
}
