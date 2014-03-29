#include "inputmanager.h"

#include <SDL.h>

namespace Input
{
    InputManager* InputManager::instance = NULL;

    void doNothing_keyPress(Key k){}
    void doNothing_keyRelease(Key k){}
    void doNothing_mouseClick(uint32_t x, uint32_t y, Key k){}
    void doNothing_mouseRelease(uint32_t x, uint32_t y, Key k){}
    void doNothing_mouseMove(uint32_t x, uint32_t y){}
    
    #define getFunc(f) f ? f : doNothing_##f

    InputManager::InputManager(boost::function<void(Key)> keyPress, boost::function<void(Key)> keyRelease,
        boost::function<void(uint32_t, uint32_t, Key)> mouseClick,
        boost::function<void(uint32_t, uint32_t, Key)> mouseRelease,
        boost::function<void(uint32_t, uint32_t)> mouseMove):
            
            mKeyPress(getFunc(keyPress)), mKeyRelease(getFunc(keyRelease)), mMouseClick(getFunc(mouseClick)),
            mMouseRelease(getFunc(mouseRelease)), mMouseMove(getFunc(mouseMove)), mQueue(500)
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

            #if SDL_MAJOR_VERSION == 2 
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
            #else
                CASE(KP0);
                CASE(KP1);
                CASE(KP2);
                CASE(KP3);
                CASE(KP4);
                CASE(KP5);
                CASE(KP6);
                CASE(KP7);
                CASE(KP8);
                CASE(KP9);

                CASE(NUMLOCK);
                CASE(SCROLLOCK);

                CASE(LSUPER);
                CASE(RSUPER);

                CASE(PRINT);
            #endif

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

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    e.vals.mouseButton.key = event.button.button;
                    e.vals.mouseButton.x = event.button.x;
                    e.vals.mouseButton.y = event.button.y;
                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    e.vals.mouseMove.x = event.motion.x;
                    e.vals.mouseMove.y = event.motion.y;
                    break;
                }

                default:
                {
                    break;
                }
            }

            while(!mQueue.push(e)) {} // push, or wait until buffer not full, then push
        }
    }

    
    void InputManager::processInput()
    {
        Event event;

        while(mQueue.pop(event))
        {
            switch (event.type) 
            {
                case SDL_KEYDOWN:
                {
                    Key key = getKey(event.vals.key);
                    if(key != KEY_UNDEF)
                        mKeyPress(key);
                    break;
                }
                case SDL_KEYUP:
                {
                    Key key = getKey(event.vals.key);
                    if(key != KEY_UNDEF)
                        mKeyRelease(key);
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    Key key = getMouseKey(event.vals.mouseButton.key);

                    if(key != KEY_UNDEF)
                        mMouseClick(event.vals.mouseButton.x, event.vals.mouseButton.y, key);
                    
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    Key key = getMouseKey(event.vals.mouseButton.key);

                    if(key != KEY_UNDEF)
                        mMouseRelease(event.vals.mouseButton.x, event.vals.mouseButton.y, key);
                    
                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    mMouseMove(event.vals.mouseMove.x, event.vals.mouseMove.y);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }

    InputManager* InputManager::get()
    {
        return instance;
    }
}
