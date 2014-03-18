#include "inputmanager.h"

#include <SDL.h>

namespace Input
{
    InputManager::InputManager(boost::function<void(Key)> _keyPress, boost::function<void(Key)> _keyRelease): 
        mKeyPress(_keyPress), mKeyRelease(_keyRelease) {}

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

            CASE(NUMLOCK);
            CASE(CAPSLOCK);
            CASE(SCROLLOCK);
            CASE(RSHIFT);
            CASE(LSHIFT);
            CASE(RCTRL);
            CASE(LCTRL);
            CASE(RALT);
            CASE(LALT);
            CASE(RMETA);
            CASE(LMETA);
            CASE(LSUPER);
            CASE(RSUPER);
            CASE(MODE);
            CASE(COMPOSE);

            CASE(HELP);
            CASE(PRINT);
            CASE(SYSREQ);
            CASE(BREAK);
            CASE(MENU);
            CASE(POWER);
            CASE(EURO);
            CASE(UNDO);

            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }

        return key;

    }

    void InputManager::poll()
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type) 
            {
                case SDL_KEYDOWN:
                {
                    Key key = getKey(event.key.keysym.sym);
                    if(key != KEY_UNDEF)
                        mKeyPress(key);
                    break;
                }
                case SDL_KEYUP:
                {
                    Key key = getKey(event.key.keysym.sym);
                    if(key != KEY_UNDEF)
                        mKeyRelease(key);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
}
