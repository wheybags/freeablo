#include "engineinputmanager.h"
#include "../fagui/guimanager.h"
#include "../farender/renderer.h"
#include <functional>
#include <misc/assert.h>
#include <nuklearmisc/inputfwd.h>

namespace Engine
{
    namespace ph = std::placeholders;

    EngineInputManager::EngineInputManager(nk_context* nk_ctx)
        : mNkCtx(nk_ctx), mInput(std::bind(&EngineInputManager::keyPress, this, ph::_1),
                                 std::bind(&EngineInputManager::keyRelease, this, ph::_1),
                                 std::bind(&EngineInputManager::mouseClick, this, ph::_1, ph::_2, ph::_3, ph::_4),
                                 std::bind(&EngineInputManager::mouseRelease, this, ph::_1, ph::_2, ph::_3),
                                 std::bind(&EngineInputManager::mouseMove, this, ph::_1, ph::_2, ph::_3, ph::_4),
                                 nullptr,
                                 std::bind(&EngineInputManager::textInput, this, ph::_1))
    {
        for (int action = 0; action < static_cast<int>(KeyboardInputAction::max); action++)
        {
            KeyboardInputAction keyAction = (KeyboardInputAction)action;
            mHotkeys[keyAction] = Input::Hotkey(keyboardActionToString(keyAction));
        }
    }

    void EngineInputManager::registerKeyboardObserver(KeyboardInputObserverInterface* observer) { mKeyboardObservers.push_back(observer); }

    void EngineInputManager::registerMouseObserver(MouseInputObserverInterface* observer) { mMouseObservers.push_back(observer); }

    void EngineInputManager::setGuiManager(FAGui::GuiManager* guiManager) { mGuiManager = guiManager; }

    void EngineInputManager::notifyKeyboardObservers(KeyboardInputAction action)
    {
        for (auto observer : mKeyboardObservers)
        {
            observer->notify(action);
        }
    }

    void EngineInputManager::notifyMouseObservers(MouseInputAction action, Misc::Point mousePosition, const Input::KeyboardModifiers& modifiers)
    {
        for (auto observer : mMouseObservers)
        {
            observer->notify(action, mousePosition, mMouseDown, modifiers);
        }
    }

    void EngineInputManager::keyPress(Input::Key key)
    {
        NuklearMisc::handleNuklearKeyboardEvent(mNkCtx, true, key, mKbMods);

        switch (key)
        {
            case Input::Key::KEY_RSHIFT:;
            case Input::Key::KEY_LSHIFT:;
            case Input::Key::KEY_RCTRL:;
            case Input::Key::KEY_LCTRL:;
            case Input::Key::KEY_RALT:;
            case Input::Key::KEY_LALT:;
            case Input::Key::KEY_RSUPER:;
            case Input::Key::KEY_LSUPER:;
            case Input::Key::KEY_NUMLOCK:;
            case Input::Key::KEY_SCROLLOCK:
                return;
            default:
            {
                break;
            }
        }

        Input::Hotkey hotkey;
        hotkey.key = (int)key;
        Input::InputManager& input = *Input::InputManager::get();

        Input::KeyboardModifiers modifiers = input.getModifiers();

        hotkey.ctrl = modifiers.ctrl;
        hotkey.shift = modifiers.shift;
        hotkey.alt = modifiers.alt;
        for (auto observer : mKeyboardObservers)
        {
            observer->keyPress(hotkey);
        }

        for (int action = 0; action < static_cast<int>(KeyboardInputAction::max); action++)
        {
            KeyboardInputAction keyAction = (KeyboardInputAction)action;
            // quite possibly certain hotkeys could actually be triggered on keyRelease rather than keyPress
            if (hotkey == getHotkey(keyAction))
            {
                notifyKeyboardObservers(keyAction);
            }
        }
    }

    void EngineInputManager::keyRelease(Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(mNkCtx, false, key, mKbMods); }

    void EngineInputManager::textInput(std::string inp) { NuklearMisc::handleNuklearTextInputEvent(mNkCtx, inp); }

    void EngineInputManager::setHotkey(KeyboardInputAction action, Input::Hotkey hotkey)
    {
        auto actionAsString = keyboardActionToString(action);
        mHotkeys[action] = hotkey;
        mHotkeys[action].save(actionAsString.c_str());
    }

    Input::Hotkey EngineInputManager::getHotkey(KeyboardInputAction action) { return mHotkeys[action]; }

    std::vector<Input::Hotkey> EngineInputManager::getHotkeys()
    {
        std::vector<Input::Hotkey> hotkeys;

        for (auto it = mHotkeys.begin(); it != mHotkeys.end(); it++)
        {
            hotkeys.push_back(it->second);
        }
        return hotkeys;
    }

    void EngineInputManager::mouseClick(int32_t x, int32_t y, Input::Key key, bool isDoubleClick)
    {
        NuklearMisc::handleNuklearMouseEvent(mNkCtx, x, y, key, true, isDoubleClick);

        if (key == Input::Key::KEY_LEFT_MOUSE)
        {
            mMousePosition = Misc::Point{x, y};
            mMouseDown = true;
            mClick = true;
        }
    }

    void EngineInputManager::mouseRelease(int32_t x, int32_t y, Input::Key key)
    {
        NuklearMisc::handleNuklearMouseEvent(mNkCtx, x, y, key, false, false);

        if (key == Input::Key::KEY_LEFT_MOUSE)
            mMouseDown = false;

        if (!nk_item_is_any_active(mNkCtx) && !mGuiManager->isModalDlgShown())
            notifyMouseObservers(MouseInputAction::MOUSE_RELEASE, mMousePosition, mKbMods);
    }

    void EngineInputManager::mouseMove(int32_t x, int32_t y, int32_t xrel, int32_t yrel)
    {
        NuklearMisc::handleNuklearMouseMoveEvent(mNkCtx, x, y, xrel, yrel);

        if (!nk_item_is_any_active(mNkCtx))
            notifyMouseObservers(MouseInputAction::MOUSE_MOVE, mMousePosition, mKbMods);

        mMousePosition = Misc::Point{x, y};
    }

    std::string EngineInputManager::keyboardActionToString(KeyboardInputAction action) const
    {
        switch (action)
        {
            case KeyboardInputAction::pause:
                return "Pause";
            case KeyboardInputAction::quit:
                return "Quit";
            case KeyboardInputAction::noclip:
                return "Noclip";
            case KeyboardInputAction::dataChangeLevelUp:
                return "Changelvlup";
            case KeyboardInputAction::dataChangeLevelDown:
                return "Changelvldwn";
            case KeyboardInputAction::toggleConsole:
                return "ToggleConsole";
            case KeyboardInputAction::nextOption:
                return "NextOption";
            case KeyboardInputAction::prevOption:
                return "PrevOption";
            case KeyboardInputAction::accept:
                return "Accept";
            case KeyboardInputAction::reject:
                return "Reject";
            case KeyboardInputAction::toggleCharacterInfo:
                return "ToggleCharacterInfo";
            case KeyboardInputAction::toggleQuests:
                return "ToggleQuests";
            case KeyboardInputAction::toggleSpellbook:
                return "ToggleSpellbook";
            case KeyboardInputAction::toggleInventory:
                return "ToggleInventory";

            case KeyboardInputAction::max:
                break;
        }
        release_assert(false && "Invalid enum value passed to keyboardActionToString");
        return "";
    }

    void EngineInputManager::update(bool paused)
    {
        mKbMods = mInput.getModifiers();
        mPaused = paused;

        nk_input_begin(mNkCtx);
        bool quit = mInput.processInput();

        // TODO: bit nasty to use keybard observers for this, but meh
        if (quit)
            notifyKeyboardObservers(KeyboardInputAction::quit);

        nk_input_end(mNkCtx);

        if (!paused && mMouseDown && !nk_item_is_any_active(mNkCtx) && !mGuiManager->isModalDlgShown())
        {
            if (mClick)
                notifyMouseObservers(MouseInputAction::MOUSE_DOWN, mMousePosition, mKbMods);

            mClick = false;
        }
    }
}
