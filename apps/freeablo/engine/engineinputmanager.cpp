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

    void EngineInputManager::notifyMouseObservers(MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers)
    {
        for (auto observer : mMouseObservers)
        {
            observer->notify(action, mousePosition, mouseDown, modifiers);
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

        mMousePosition = Misc::Point{x, y};

        // Only pass relevant mouse clicks to game engine.
        if (!mPaused && !nk_item_is_any_active(mNkCtx) && !mGuiManager->isModalDlgShown())
        {
            if (key == Input::Key::KEY_LEFT_MOUSE)
            {
                mMouseDown = true;
                mClick = true;
            }
            if (key == Input::Key::KEY_RIGHT_MOUSE)
            {
                mRightMouseDown = true;
                mRightClick = true;
            }
        }
    }

    void EngineInputManager::mouseRelease(int32_t x, int32_t y, Input::Key key)
    {
        NuklearMisc::handleNuklearMouseEvent(mNkCtx, x, y, key, false, false);

        if (key == Input::Key::KEY_LEFT_MOUSE && mMouseDown)
        {
            mMouseDown = false;
            if (!nk_item_is_any_active(mNkCtx) && !mGuiManager->isModalDlgShown())
                notifyMouseObservers(MouseInputAction::MOUSE_RELEASE, mMousePosition, mMouseDown, mKbMods);
        }
        if (key == Input::Key::KEY_RIGHT_MOUSE && mRightMouseDown)
        {
            mRightMouseDown = false;
            if (!nk_item_is_any_active(mNkCtx) && !mGuiManager->isModalDlgShown())
                notifyMouseObservers(MouseInputAction::RIGHT_MOUSE_RELEASE, mMousePosition, mRightMouseDown, mKbMods);
        }
    }

    void EngineInputManager::mouseMove(int32_t x, int32_t y, int32_t xrel, int32_t yrel)
    {
        NuklearMisc::handleNuklearMouseMoveEvent(mNkCtx, x, y, xrel, yrel);

        mMousePosition = Misc::Point{x, y};

        if (!nk_item_is_any_active(mNkCtx))
            notifyMouseObservers(MouseInputAction::MOUSE_MOVE, mMousePosition, mMouseDown, mKbMods);
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
            case KeyboardInputAction::toggleSpellSelection:
                return "ToggleSpellSelection";
            case KeyboardInputAction::spellHotkeyF5:
                return "SpellHotkeyF5";
            case KeyboardInputAction::spellHotkeyF6:
                return "SpellHotkeyF6";
            case KeyboardInputAction::spellHotkeyF7:
                return "SpellHotkeyF7";
            case KeyboardInputAction::spellHotkeyF8:
                return "SpellHotkeyF8";

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

        // TODO: bit nasty to use keyboard observers for this, but meh
        if (quit)
            notifyKeyboardObservers(KeyboardInputAction::quit);

        nk_input_end(mNkCtx);

        if (mMouseDown && mClick)
            notifyMouseObservers(MouseInputAction::MOUSE_DOWN, mMousePosition, mMouseDown, mKbMods);
        mClick = false;

        if (mRightMouseDown && mRightClick)
            notifyMouseObservers(MouseInputAction::RIGHT_MOUSE_DOWN, mMousePosition, mRightMouseDown, mKbMods);
        mRightClick = false;
    }
}
