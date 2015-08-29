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

    class EngineInputManager
    {
        public:
            EngineInputManager(EngineMain& engine);
            void update(bool paused);

            Input::Hotkey quit_key;
            Input::Hotkey noclip_key;
            Input::Hotkey changelvldwn_key;
            Input::Hotkey changelvlup_key;
            Input::Hotkey toggleconsole_key;

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

#endif // FA_INPUT_H

