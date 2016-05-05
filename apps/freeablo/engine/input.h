#ifndef FA_INPUT_H
#define FA_INPUT_H

#include <map>
#include <vector>
#include <string>
#include <input/hotkey.h>
#include <input/inputmanager.h>
#include "inputobserverinterface.h"

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
            EngineInputManager();
            void update(bool paused);
            void setHotkey(KeyboardInputAction action, Input::Hotkey hotkey);
            Input::Hotkey getHotkey(KeyboardInputAction action);
            std::vector<Input::Hotkey> getHotkeys();
            void registerKeyboardObserver(KeyboardInputObserverInterface* observer);
            void registerMouseObserver(MouseInputObserverInterface* observer);

        private:
            void keyPress(Input::Key key);
            void mouseClick(size_t x, size_t y, Input::Key key);
            void mouseRelease(size_t, size_t, Input::Key key);
            void mouseMove(size_t x, size_t y);
            std::string keyboardActionToString(KeyboardInputAction action) const;
            void notifyKeyboardObservers(KeyboardInputAction action);
            void notifyMouseObservers(MouseInputAction action, Point mousePosition);

            Input::InputManager mInput;
            bool mToggleConsole = false;
            Point mMousePosition;
            bool mMouseDown = false;
            bool mClick = false;
            std::map<KeyboardInputAction,Input::Hotkey> mHotkeys;
            std::vector<KeyboardInputObserverInterface*> mKeyboardObservers;
            std::vector<MouseInputObserverInterface*> mMouseObservers;
    };
}

#endif // FA_INPUT_H

