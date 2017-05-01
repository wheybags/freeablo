#include <functional>
#include "../farender/renderer.h"
#include "../fagui/console.h"
#include "engineinputmanager.h"
#include "misc/utility.h"

namespace Engine
{
    namespace ph = std::placeholders;

    EngineInputManager::EngineInputManager():
        mInput( BIND_THIS (keyPress),
                NULL,
                BIND_THIS (mouseClick),
                BIND_THIS (mouseRelease),
                BIND_THIS (mouseMove),
                FARender::Renderer::get()->getRocketContext())
    {
        for(int action = 0; action < KEYBOARD_INPUT_ACTION_MAX; action++)
        {
            KeyboardInputAction keyAction = (KeyboardInputAction)action;
            mHotkeys[keyAction] = Input::Hotkey(keyboardActionToString(keyAction));
        }
    }

    void EngineInputManager::registerKeyboardObserver(KeyboardInputObserverInterface * observer)
    {
        mKeyboardObservers.push_back(observer);
    }

    void EngineInputManager::registerMouseObserver(MouseInputObserverInterface* observer)
    {
        mMouseObservers.push_back(observer);
    }

    void EngineInputManager::notifyKeyboardObservers(KeyboardInputAction action)
    {
        for(auto observer : mKeyboardObservers)
        {
            observer->notify(action);
        }
    }

    void EngineInputManager::notifyMouseObservers(MouseInputAction action, Point mousePosition)
    {
        for(auto observer : mMouseObservers)
        {
            observer->notify(action, mousePosition);
        }
    }

    void EngineInputManager::keyPress(Input::Key key)
    {
        switch(key)
        {
            case Input::KEY_RSHIFT:;
            case Input::KEY_LSHIFT:;
            case Input::KEY_RCTRL:;
            case Input::KEY_LCTRL:;
            case Input::KEY_RALT:;
            case Input::KEY_LALT:;
            case Input::KEY_RSUPER:;
            case Input::KEY_LSUPER:;
            case Input::KEY_NUMLOCK:;
            case Input::KEY_SCROLLOCK: return;
            default:
                {
                    break;
                }
        }

        Input::Hotkey hotkey;
        hotkey.key = key;
        Input::InputManager& input = *Input::InputManager::get();

        uint32_t modifiers = input.getModifiers();

        switch(modifiers)
        {
            case 0: break;
            case 1: hotkey.ctrl = true; break;
            case 2: hotkey.alt = true; break;
            case 3: hotkey.ctrl = true; hotkey.alt = true; break;
            case 4: hotkey.shift = true; break;
            case 5: hotkey.ctrl = true; hotkey.shift = true; break;
            case 6: hotkey.alt = true; hotkey.shift = true; break;
            case 7: hotkey.ctrl = true; hotkey.alt = true; hotkey.shift = true; break;
        }

        FAGui::Console & console = FAGui::Console::getInstance(FARender::Renderer::get()->getRocketContext());

        if(hotkey == getHotkey(TOGGLE_CONSOLE))
        {
            mToggleConsole = true;
        }
        else if(console.isVisible() == false)
        {
            for(int action = 0; action < KEYBOARD_INPUT_ACTION_MAX; action++)
            {
                KeyboardInputAction keyAction = (KeyboardInputAction)action;
                if (hotkey == getHotkey(keyAction)) {
                    notifyKeyboardObservers(keyAction);
                }
            }
        }
    }

    void EngineInputManager::setHotkey(KeyboardInputAction action, Input::Hotkey hotkey)
    {
        auto actionAsString = keyboardActionToString(action);
        mHotkeys[action] = hotkey;
        mHotkeys[action].save(actionAsString.c_str());
    }

    Input::Hotkey EngineInputManager::getHotkey(KeyboardInputAction action)
    {
        return mHotkeys[action];
    }

    std::vector<Input::Hotkey> EngineInputManager::getHotkeys()
    {
        std::vector<Input::Hotkey> hotkeys;

        for(auto it = mHotkeys.begin(); it != mHotkeys.end(); it++) {
            hotkeys.push_back(it->second);
        }
        return hotkeys;
    }

    void EngineInputManager::mouseClick(size_t x, size_t y, Input::Key key)
    {
        if(key == Input::KEY_LEFT_MOUSE)
        {
            mMousePosition = Point(x,y);
            mMouseDown = true;
            mClick = true;
        }
    }

    void EngineInputManager::mouseRelease(size_t, size_t, Input::Key key)
    {
        if(key == Input::KEY_LEFT_MOUSE)
            mMouseDown = false;

        notifyMouseObservers(MOUSE_RELEASE, mMousePosition);
    }

    void EngineInputManager::mouseMove(size_t x, size_t y)
    {
        mMousePosition = Point(x,y);
       notifyMouseObservers(MOUSE_MOVE, mMousePosition);
    }

    std::string EngineInputManager::keyboardActionToString(KeyboardInputAction action) const
    {
        std::string actionAsString;

        switch(action)
        {
            case QUIT:
                actionAsString = "Quit";
                break;
            case NOCLIP:
                actionAsString = "Noclip";
                break;
            case CHANGE_LEVEL_UP:
                actionAsString = "Changelvlup";
                break;
            case CHANGE_LEVEL_DOWN:
                actionAsString = "Changelvldwn";
                break;
            case TOGGLE_CONSOLE:
                actionAsString = "ToggleConsole";
                break;
            default:
                actionAsString = "Unknown";
                break;
        }

        return actionAsString;
    }

    void EngineInputManager::update(bool paused)
    {
        mInput.processInput(paused);
        if(mToggleConsole)
        {
            FAGui::Console & console = FAGui::Console::getInstance(FARender::Renderer::get()->getRocketContext());
            console.toggle();
            mToggleConsole = false;
        }

        if(!paused && mMouseDown)
        {
            notifyMouseObservers(MOUSE_DOWN, mMousePosition);

            if(mClick)
                notifyMouseObservers(MOUSE_CLICK, mMousePosition);

            mClick = false;
        }
    }
}
