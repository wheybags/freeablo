#ifndef FA_INPUT_H
#define FA_INPUT_H

#include <map>
#include <vector>
#include <string>
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

            enum Action {
                QUIT,
                NOCLIP,
                CHANGE_LEVEL_DOWN,
                CHANGE_LEVEL_UP,
                TOGGLE_CONSOLE
            };

            EngineInputManager(EngineMain& engine);
            void update(bool paused);
            void setHotkey(Action action, Input::Hotkey hotkey);
            Input::Hotkey getHotkey(Action action);
            std::vector<Input::Hotkey> getHotkeys();

        private:
            void keyPress(Input::Key key);
            void mouseClick(size_t x, size_t y, Input::Key key);
            void mouseRelease(size_t, size_t, Input::Key key);
            void mouseMove(size_t x, size_t y);
            std::string actionToString(Action action) const;

            Input::InputManager mInput;
            EngineMain& mEngine;
            bool mToggleConsole = false;
            size_t mXClick = 0;
            size_t mYClick = 0;
            bool mMouseDown = false;
            bool mClick = false;
            std::map<Action,Input::Hotkey> mHotkeys;
    };
}

#endif // FA_INPUT_H

