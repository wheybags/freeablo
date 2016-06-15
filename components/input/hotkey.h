#ifndef HOTKEY_H
#define	HOTKEY_H

#include <string>

namespace Input
{
    class Hotkey
    {
        public:
            static void initializePythonWrapper();

            Hotkey();
            Hotkey(const char *name);
            Hotkey(const std::string& name);
            Hotkey(int nkey, bool nshift, bool nctrl, bool nalt);
            std::string name() const;
            bool operator==(const Hotkey &other);
            void save(const char *name);

            int key;
            bool shift, ctrl, alt;
    };
}

#endif	/* HOTKEY_H */

