#include <functional>
#include "../farender/renderer.h"
#include "engineinputmanager.h"

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

    void handleNuklearKeyboardEvent(nk_context* ctx, bool isDown, Input::Key sym, KeyboardModifiers mods)
    {
        int down = isDown;

        if (sym == Input::KEY_RSHIFT || sym == Input::KEY_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == Input::KEY_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == Input::KEY_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == Input::KEY_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == Input::KEY_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == Input::KEY_HOME) {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        }
        else if (sym == Input::KEY_END) {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        }
        else if (sym == Input::KEY_PAGEDOWN) {
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        }
        else if (sym == Input::KEY_PAGEUP) {
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        }
        else if (sym == Input::KEY_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && mods.ctrl);
        else if (sym == Input::KEY_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && mods.ctrl);
        else if (sym == Input::KEY_c)
            nk_input_key(ctx, NK_KEY_COPY, down && mods.ctrl);
        else if (sym == Input::KEY_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && mods.ctrl);
        else if (sym == Input::KEY_x)
            nk_input_key(ctx, NK_KEY_CUT, down && mods.ctrl);
        else if (sym == Input::KEY_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && mods.ctrl);
        else if (sym == Input::KEY_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && mods.ctrl);
        else if (sym == Input::KEY_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == Input::KEY_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == Input::KEY_LEFT) {
            if (mods.ctrl)
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else nk_input_key(ctx, NK_KEY_LEFT, down);
        }
        else if (sym == Input::KEY_RIGHT) {
            if (mods.ctrl)
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else nk_input_key(ctx, NK_KEY_RIGHT, down);
        }
    }

    void EngineInputManager::keyPress(Input::Key key)
    {
        handleNuklearKeyboardEvent(mNkCtx, true, key, mKbMods);

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
        handleNuklearKeyboardEvent(mNkCtx, false, key, mKbMods);
    }

    void EngineInputManager::textInput(std::string inp)
    {
        nk_glyph glyph;
        memcpy(glyph, inp.c_str(), NK_UTF_SIZE);
        nk_input_glyph(mNkCtx, glyph);
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

    void handleNuklearMouseEvent(nk_context* ctx, int32_t x, int32_t y, Input::Key key, bool isDown, bool isDoubleClick)
    {
        int down = isDown;

        if (key == Input::KEY_LEFT_MOUSE)
        {
            if (isDoubleClick)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        }
        else if (key == Input::KEY_MIDDLE_MOUSE)
        {
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        }
        else if (key == Input::KEY_RIGHT_MOUSE)
        {
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        }
    }

    void EngineInputManager::mouseClick(int32_t x, int32_t y, Input::Key key, bool isDoubleClick)
    {
        handleNuklearMouseEvent(mNkCtx, x, y, key, true, isDoubleClick);

        if(key == Input::KEY_LEFT_MOUSE)
        {
            mMousePosition = Misc::Point{x,y};
            mMouseDown = true;
            mClick = true;
        }
    }

    void EngineInputManager::mouseRelease(int32_t x, int32_t y, Input::Key key)
    {
        handleNuklearMouseEvent(mNkCtx, x, y, key, false, false);

        if(key == Input::KEY_LEFT_MOUSE)
            mMouseDown = false;

        if (!nk_item_is_any_active (mNkCtx))
            notifyMouseObservers(MOUSE_RELEASE, mMousePosition);
    }

    void EngineInputManager::mouseMove(int32_t x, int32_t y, int32_t xrel, int32_t yrel)
    {
        if (mNkCtx->input.mouse.grabbed)
        {
            int mouse_prev_x = (int)mNkCtx->input.mouse.prev.x, mouse_prev_y = (int)mNkCtx->input.mouse.prev.y;
            nk_input_motion(mNkCtx, mouse_prev_x + xrel, mouse_prev_y + yrel);
        }
        else
        {
            nk_input_motion(mNkCtx, x, y);
        }

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

        uint32_t modifiers = mInput.getModifiers();

        mKbMods = KeyboardModifiers();

        switch (modifiers)
        {
            case 0: break;
            case 1: mKbMods.ctrl = true; break;
            case 2: mKbMods.alt = true; break;
            case 3: mKbMods.ctrl = true; mKbMods.alt = true; break;
            case 4: mKbMods.shift = true; break;
            case 5: mKbMods.ctrl = true; mKbMods.shift = true; break;
            case 6: mKbMods.alt = true; mKbMods.shift = true; break;
            case 7: mKbMods.ctrl = true; mKbMods.alt = true; mKbMods.shift = true; break;
        }

        nk_input_begin(mNkCtx);
        mInput.processInput(paused);
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
