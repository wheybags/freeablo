#ifndef FA_INPUT_H
#define FA_INPUT_H

#include <input/hotkey.h>
#include <input/inputmanager.h>

namespace FAWorld
{
    class World;
}

namespace Engine
{
    class EngineMain;

    extern Input::Hotkey quit_key;
    extern Input::Hotkey noclip_key;
    extern Input::Hotkey changelvldwn_key;
    extern Input::Hotkey changelvlup_key;
    extern Input::Hotkey toggleconsole_key;

    class EngineInputManager
    {
        public:
            EngineInputManager(EngineMain& engine);
            void update(bool paused);

        private:
            void keyPress(Input::Key key);
            void mouseClick(size_t x, size_t y, Input::Key key);
            void mouseRelease(size_t, size_t, Input::Key key);
            void mouseMove(size_t x, size_t y);

            Input::InputManager mInput;

            bool mToggleConsole = false;
            EngineMain& mEngine;

            size_t mXClick = 0, mYClick = 0;
            bool mMouseDown = false;
            bool mClick = false;
    };
}

#endif // INPUT_H

