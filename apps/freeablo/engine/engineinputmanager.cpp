#include <functional>
#include "../farender/renderer.h"
#include "engineinputmanager.h"
#include <nuklearmisc/inputfwd.h>

namespace Engine
{
    namespace ph = std::placeholders;

    EngineInputManager::EngineInputManager(nk_context* nk_ctx):
        mNkCtx(nk_ctx),
        mInput( std::bind(&EngineInputManager::keyPress,this, ph::_1),
                std::bind(&EngineInputManager::keyRelease, this, ph::_1),
                std::bind(&EngineInputManager::mouseClick, this, ph::_1, ph::_2, ph::_3, ph::_4),
                std::bind(&EngineInputManager::mouseRelease, this, ph::_1, ph::_2, ph::_3),
                std::bind(&EngineInputManager::mouseMove, this, ph::_1, ph::_2, ph::_3, ph::_4),
                nullptr,
                std::bind(&EngineInputManager::textInput, this, ph::_1))
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

    void EngineInputManager::notifyMouseObservers(MouseInputAction action, Misc::Point mousePosition)
    {
        for(auto observer : mMouseObservers)
        {
            observer->notify(action, mousePosition);
        }
    }

    void EngineInputManager::keyPress(Input::Key key)
    {
        NuklearMisc::handleNuklearKeyboardEvent(mNkCtx, true, key, mKbMods);

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

        Input::KeyboardModifiers modifiers = input.getModifiers();

        hotkey.ctrl = modifiers.ctrl;
        hotkey.shift = modifiers.shift;
        hotkey.alt = modifiers.alt;

        for(int action = 0; action < KEYBOARD_INPUT_ACTION_MAX; action++)
        {
            KeyboardInputAction keyAction = (KeyboardInputAction)action;
            if (hotkey == getHotkey(keyAction)) {
                notifyKeyboardObservers(keyAction);
            }
        }
    }

    void EngineInputManager::keyRelease(Input::Key key)
    {
        NuklearMisc::handleNuklearKeyboardEvent(mNkCtx, false, key, mKbMods);
    }

    void EngineInputManager::textInput(std::string inp)
    {
        NuklearMisc::handleNuklearTextInputEvent(mNkCtx, inp);
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



    void EngineInputManager::mouseClick(int32_t x, int32_t y, Input::Key key, bool isDoubleClick)
    {
        NuklearMisc::handleNuklearMouseEvent(mNkCtx, x, y, key, true, isDoubleClick);

        if(key == Input::KEY_LEFT_MOUSE)
        {
            mMousePosition = Misc::Point{x,y};
            mMouseDown = true;
            mClick = true;
        }
    }

    void EngineInputManager::mouseRelease(int32_t x, int32_t y, Input::Key key)
    {
        NuklearMisc::handleNuklearMouseEvent(mNkCtx, x, y, key, false, false);

        if(key == Input::KEY_LEFT_MOUSE)
            mMouseDown = false;

        if (!nk_item_is_any_active (mNkCtx))
            notifyMouseObservers(MOUSE_RELEASE, mMousePosition);
    }

    void EngineInputManager::mouseMove(int32_t x, int32_t y, int32_t xrel, int32_t yrel)
    {
        NuklearMisc::handleNuklearMouseMoveEvent(mNkCtx, x, y, xrel, yrel);

        if (!nk_item_is_any_active (mNkCtx))
            notifyMouseObservers(MOUSE_MOVE, mMousePosition);

        mMousePosition = Misc::Point{x,y};
    }

    std::string EngineInputManager::keyboardActionToString(KeyboardInputAction action) const
    {
        std::string actionAsString;

        switch(action)
        {
            case PAUSE:
                actionAsString = "Pause";
                break;
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
                assert(false && "Invalid enum value passed to keyboardActionToString");
                break;
        }

        return actionAsString;
    }

    void EngineInputManager::update(bool paused)
    {
        mKbMods = mInput.getModifiers();

        nk_input_begin(mNkCtx);
        bool quit = mInput.processInput(paused);

        //TODO: bit nasty to use keybard observers for this, but meh
        if(quit)
             notifyKeyboardObservers(KeyboardInputAction::QUIT);

        nk_input_end(mNkCtx);

        if(!paused && mMouseDown && !nk_item_is_any_active (mNkCtx))
        {
            notifyMouseObservers(MOUSE_DOWN, mMousePosition);

            if(mClick)
                notifyMouseObservers(MOUSE_CLICK, mMousePosition);

            mClick = false;
        }
    }
}
