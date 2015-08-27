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
    extern bool done;
    extern bool paused;
    extern bool noclip;

    extern size_t xClick, yClick;
    extern bool mouseDown;
    extern bool click;

    extern Input::Hotkey quit_key;
    extern Input::Hotkey noclip_key;
    extern Input::Hotkey changelvldwn_key;
    extern Input::Hotkey changelvlup_key;

    class EngineInputManager
    {
        public:
            EngineInputManager();
            void update(bool paused);

        private:
            void keyPress(Input::Key key);
            void mouseClick(size_t x, size_t y, Input::Key key);
            void mouseRelease(size_t, size_t, Input::Key key);
            void mouseMove(size_t x, size_t y);

            Input::InputManager mInput;
    };
}

#endif // INPUT_H

