#ifndef FA_INPUT_H
#define FA_INPUT_H

#include <map>
#include <vector>
#include <string>
#include <input/hotkey.h>
#include <input/inputmanager.h>
#include "../components/misc/misc.h"
#include "inputobserverinterface.h"
#include <fa_nuklear.h>

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
            EngineInputManager(nk_context* nk_ctx);
            void update(bool paused);
            void setHotkey(KeyboardInputAction action, Input::Hotkey hotkey);
            Input::Hotkey getHotkey(KeyboardInputAction action);
            std::vector<Input::Hotkey> getHotkeys();
            void registerKeyboardObserver(KeyboardInputObserverInterface* observer);
            void registerMouseObserver(MouseInputObserverInterface* observer);
            Input::KeyboardModifiers getKeyboardModifiers()
            {
                return mKbMods;
            }

        private:
            EngineInputManager(const EngineInputManager&);
            EngineInputManager& operator=(const EngineInputManager&);
            void keyPress(Input::Key key);
            void keyRelease(Input::Key key);
            void textInput(std::string inp);
            void mouseClick(int32_t x, int32_t y, Input::Key key, bool isDoubleClick);
            void mouseRelease(int32_t, int32_t, Input::Key key);
            void mouseMove(int32_t x, int32_t y, int32_t xrel, int32_t yrel);
            std::string keyboardActionToString(KeyboardInputAction action) const;
            void notifyKeyboardObservers(KeyboardInputAction action);
            void notifyMouseObservers(MouseInputAction action, Misc::Point mousePosition);

            nk_context* mNkCtx = nullptr;
            Input::InputManager mInput;
            Misc::Point mMousePosition;
            bool mMouseDown = false;
            bool mClick = false;
            Input::KeyboardModifiers mKbMods;
            std::map<KeyboardInputAction,Input::Hotkey> mHotkeys;
            std::vector<KeyboardInputObserverInterface*> mKeyboardObservers;
            std::vector<MouseInputObserverInterface*> mMouseObservers;
    };
}

#endif // FA_INPUT_H

