#ifndef HOTKEY_H
#define	HOTKEY_H

namespace Input
{
    class Hotkey
    {
        public:
            int key;
            bool shift, ctrl, alt;

            Hotkey();
            Hotkey(const char *name);
            Hotkey(int nkey, bool nshift, bool nctrl, bool nalt);
            bool operator==(const Hotkey &other);
            void save(const char *name);
            static void initpythonwrapper();
    };
}

#endif	/* HOTKEY_H */

