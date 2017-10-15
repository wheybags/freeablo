#include "inputmanager.h"

#include <SDL.h>

#include <render/render.h>

#include <iostream>



namespace Input
{
    InputManager* InputManager::instance = NULL;

    void doNothing_keyPress(Key){}
    void doNothing_keyRelease(Key){}
    void doNothing_mouseClick(uint32_t, uint32_t, Key, bool){}
    void doNothing_mouseRelease(uint32_t, uint32_t, Key){}
    void doNothing_mouseMove(uint32_t, uint32_t, uint32_t, uint32_t) {}
    void doNothing_mouseWheel(int32_t, int32_t) {}
    void doNothing_textInput(std::string){}
    
    #define getFunc(f) f ? f : doNothing_##f

    InputManager::InputManager(std::function<void(Key)> keyPress, std::function<void(Key)> keyRelease,
        std::function<void(uint32_t, uint32_t, Key, bool)> mouseClick,
        std::function<void(uint32_t, uint32_t, Key)> mouseRelease,
        std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> mouseMove,
        std::function<void(int32_t, int32_t)> mouseWheel,
        std::function<void(std::string)> textInput)
            : mKeyPress(getFunc(keyPress))
            , mKeyRelease(getFunc(keyRelease))
            , mMouseClick(getFunc(mouseClick))
            , mMouseRelease(getFunc(mouseRelease))
            , mMouseMove(getFunc(mouseMove))
            , mMouseWheel(getFunc(mouseWheel))
            , mTextInput(getFunc(textInput))
        {
            assert(!instance);
            instance = this;
        }

    #define CASE(val) case SDLK_##val: key = KEY_##val; break; 

    Key getKey(int sdlk)
    {
        Key key;
        
        switch(sdlk)
        {
            CASE(BACKSPACE);
            CASE(TAB);
            CASE(CLEAR);
            CASE(RETURN);
            CASE(PAUSE);
            CASE(ESCAPE);
            CASE(SPACE);
            CASE(EXCLAIM);
            CASE(QUOTEDBL);
            CASE(HASH);
            CASE(DOLLAR);
            CASE(AMPERSAND);
            CASE(QUOTE);
            CASE(LEFTPAREN);
            CASE(RIGHTPAREN);
            CASE(ASTERISK);
            CASE(PLUS);
            CASE(COMMA);
            CASE(MINUS);
            CASE(PERIOD);
            CASE(SLASH);
            CASE(0);
            CASE(1);
            CASE(2);
            CASE(3);
            CASE(4);
            CASE(5);
            CASE(6);
            CASE(7);
            CASE(8);
            CASE(9);
            CASE(COLON);
            CASE(SEMICOLON);
            CASE(LESS);
            CASE(EQUALS);
            CASE(GREATER);
            CASE(QUESTION);
            CASE(AT);

            CASE(LEFTBRACKET);
            CASE(BACKSLASH);
            CASE(RIGHTBRACKET);
            CASE(CARET);
            CASE(UNDERSCORE);
            CASE(BACKQUOTE);
            CASE(a);
            CASE(b);
            CASE(c);
            CASE(d);
            CASE(e);
            CASE(f);
            CASE(g);
            CASE(h);
            CASE(i);
            CASE(j);
            CASE(k);
            CASE(l);
            CASE(m);
            CASE(n);
            CASE(o);
            CASE(p);
            CASE(q);
            CASE(r);
            CASE(s);
            CASE(t);
            CASE(u);
            CASE(v);
            CASE(w);
            CASE(x);
            CASE(y);
            CASE(z);
            CASE(DELETE);

            CASE(KP_PERIOD);
            CASE(KP_DIVIDE);
            CASE(KP_MULTIPLY);
            CASE(KP_MINUS);
            CASE(KP_PLUS);
            CASE(KP_ENTER);
            CASE(KP_EQUALS);

            CASE(UP);
            CASE(DOWN);
            CASE(RIGHT);
            CASE(LEFT);
            CASE(INSERT);
            CASE(HOME);
            CASE(END);
            CASE(PAGEUP);
            CASE(PAGEDOWN);

            CASE(F1);
            CASE(F2);
            CASE(F3);
            CASE(F4);
            CASE(F5);
            CASE(F6);
            CASE(F7);
            CASE(F8);
            CASE(F9);
            CASE(F10);
            CASE(F11);
            CASE(F12);
            CASE(F13);
            CASE(F14);
            CASE(F15);

            CASE(CAPSLOCK);
            CASE(RSHIFT);
            CASE(LSHIFT);
            CASE(RCTRL);
            CASE(LCTRL);
            CASE(RALT);
            CASE(LALT);
            CASE(MODE);

            CASE(MENU);
            CASE(POWER);
            CASE(UNDO);

            case SDLK_KP_0: key = KEY_KP0; break;
            case SDLK_KP_1: key = KEY_KP1; break;
            case SDLK_KP_2: key = KEY_KP2; break;
            case SDLK_KP_3: key = KEY_KP3; break;
            case SDLK_KP_4: key = KEY_KP4; break;
            case SDLK_KP_5: key = KEY_KP5; break;
            case SDLK_KP_6: key = KEY_KP6; break;
            case SDLK_KP_7: key = KEY_KP7; break;
            case SDLK_KP_8: key = KEY_KP8; break;
            case SDLK_KP_9: key = KEY_KP9; break;

            case SDLK_NUMLOCKCLEAR: key = KEY_NUMLOCK; break;
            case SDLK_SCROLLLOCK: key = KEY_SCROLLOCK; break;

            case SDLK_RGUI: key = KEY_RSUPER; break;
            case SDLK_LGUI: key = KEY_LSUPER; break;

            case SDLK_PRINTSCREEN: key = KEY_PRINT; break;
         
            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }

        return key;

    }

    Key getMouseKey(int sdlk)
    {
        switch(sdlk)
        {
            case SDL_BUTTON_LEFT:
                return KEY_LEFT_MOUSE;
            case SDL_BUTTON_RIGHT:
                return KEY_RIGHT_MOUSE;
            case SDL_BUTTON_MIDDLE:
                return KEY_MIDDLE_MOUSE;
            default:
                return KEY_UNDEF;
        }
    }
   
    
    void InputManager::poll()
    {
        SDL_Event event;
        
        Event e;

        while(SDL_PollEvent(&event))
        {   
            e.type = event.type;

            switch (event.type) 
            {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    e.vals.key = event.key.keysym.sym;
                    break;
                }

                case SDL_TEXTINPUT:
                {
                    e.vals.textInput.text = new std::string(event.text.text);
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    e.vals.mouseButton.key = event.button.button;
                    e.vals.mouseButton.x = event.button.x;
                    e.vals.mouseButton.y = event.button.y;
                    e.vals.mouseButton.numClicks = event.button.clicks;
                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    e.vals.mouseMove.x = event.motion.x;
                    e.vals.mouseMove.y = event.motion.y;
                    e.vals.mouseMove.xrel = event.motion.xrel;
                    e.vals.mouseMove.yrel = event.motion.yrel;
                    break;
                }

                case SDL_MOUSEWHEEL:
                {
                    e.vals.mouseWheel.x = event.wheel.x;
                    e.vals.mouseWheel.y = event.wheel.y;
                    break;
                }

                case SDL_WINDOWEVENT:
                {
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                        Render::resize(event.window.data1, event.window.data2);

                    break;
                }

                case SDL_QUIT:
                {
                    if(!mHasQuit)
                        mHasQuit = true;
                    else // quit immediately if user asks for quit twice
                        exit(0);
                    break;
                }

                default:
                {
                    break;
                }
            }

            while(!mQueue.push(e)) {} // push, or wait until buffer not full, then push
        }

        SDL_Keymod sdlMods = SDL_GetModState();

        KeyboardModifiers mods;

        if(sdlMods & KMOD_CTRL)
            mods.ctrl = true;

        if(sdlMods & KMOD_SHIFT)
            mods.shift = true;

        if(sdlMods & KMOD_ALT)
            mods.alt = true;

        mModifiers = mods;
    }
    
    KeyboardModifiers InputManager::getModifiers()
    {
        return mModifiers;
    }

    bool InputManager::processInput(bool paused)
    {
        Event event;

        bool quit = false;

        while(mQueue.pop(event))
        {
            switch (event.type) 
            {
                case SDL_KEYDOWN:
                {
                    Key key = getKey(event.vals.key);
                    if(key != KEY_UNDEF)
                    {
                        if(!paused)
                            mKeyPress(key);
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    Key key = getKey(event.vals.key);
                    if(key != KEY_UNDEF)
                    {
                        if(!paused)
                            mKeyRelease(key);
                    }
                    break;
                }

                case SDL_TEXTINPUT:
                {
                    mTextInput(*event.vals.textInput.text);
                    delete event.vals.textInput.text;
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    Key key = getMouseKey(event.vals.mouseButton.key);

                    if(key != KEY_UNDEF)
                        mMouseClick(event.vals.mouseButton.x, event.vals.mouseButton.y, key, event.vals.mouseButton.numClicks > 1);
                    
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    Key key = getMouseKey(event.vals.mouseButton.key);
                    mMouseRelease(event.vals.mouseButton.x, event.vals.mouseButton.y, key);

                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    mMouseMove(event.vals.mouseMove.x, event.vals.mouseMove.y, event.vals.mouseMove.xrel, event.vals.mouseMove.yrel);
                    break;
                }

                case SDL_MOUSEWHEEL:
                {
                    mMouseWheel(event.vals.mouseWheel.x, event.vals.mouseWheel.y);
                    break;
                }

                case SDL_QUIT:
                {
                    quit = true;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        return quit;
    }

    InputManager* InputManager::get()
    {
        return instance;
    }
}
