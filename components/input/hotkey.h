#ifndef HOTKEY_H
#define	HOTKEY_H

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

namespace Input
{
    class Hotkey
    {
        public:
            int key;
            bool shift, ctrl, alt;

            Hotkey();
            Hotkey(const char *name, bpt::ptree hotkeypt);
            Hotkey(int nkey, bool nshift, bool nctrl, bool nalt);
            bool operator==(const Hotkey &other);
            void save(const char *name, bpt::ptree hotkeypt);
            static void initpythonwrapper();
    };
}

#endif	/* HOTKEY_H */

