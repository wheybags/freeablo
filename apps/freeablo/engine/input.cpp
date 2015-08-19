#include "input.h"

namespace Engine
{
    bool done = false;
    bool paused = false;
    bool noclip = false;
    int changeLevel = 0;

    size_t xClick = 0, yClick = 0;
    bool mouseDown = false;
    bool click = false;

    Input::Hotkey quit_key;
    Input::Hotkey noclip_key;
    Input::Hotkey changelvldwn_key;
    Input::Hotkey changelvlup_key;

    void keyPress(Input::Key key)
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

        if (hotkey == quit_key)
        {
            done = true;
            return;
        }

        if (hotkey == noclip_key)
        {
            noclip = !noclip;
            return;
        }

        if (hotkey == changelvlup_key)
        {
            changeLevel = -1;
            return;
        }

        if (hotkey == changelvldwn_key)
        {
            changeLevel = 1;
            return;
        }
    }

    void mouseClick(size_t x, size_t y, Input::Key key)
    {
        if(key == Input::KEY_LEFT_MOUSE)
        {
            xClick = x;
            yClick = y;
            mouseDown = true;
            click = true;
        }
    }

    void mouseRelease(size_t, size_t, Input::Key key)
    {
        if(key == Input::KEY_LEFT_MOUSE)
            mouseDown = false;
    }

    void mouseMove(size_t x, size_t y)
    {
        xClick = x;
        yClick = y;
    }
}
