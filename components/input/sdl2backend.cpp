#include "inputmanager.h"

#include <SDL.h>

#include <render/render.h>

#include <misc/boost_python.h>

#include <iostream>

namespace Input
{
    InputManager* InputManager::instance = NULL;

    void doNothing_keyPress(Key){}
    void doNothing_keyRelease(Key){}
    void doNothing_mouseClick(uint32_t, uint32_t, Key){}
    void doNothing_mouseRelease(uint32_t, uint32_t, Key){}
    void doNothing_mouseMove(uint32_t, uint32_t){}
    
    #define getFunc(f) f ? f : doNothing_##f

    void baseClickedHelper()
    {
        InputManager::get()->rocketBaseClicked();
    }

    BOOST_PYTHON_MODULE(freeablo_input)
    {
        boost::python::def("baseClicked", &baseClickedHelper);
    }

    InputManager::InputManager(std::function<void(Key)> keyPress, std::function<void(Key)> keyRelease,
        std::function<void(uint32_t, uint32_t, Key)> mouseClick,
        std::function<void(uint32_t, uint32_t, Key)> mouseRelease,
        std::function<void(uint32_t, uint32_t)> mouseMove,
        Rocket::Core::Context* context):
            
            mKeyPress(getFunc(keyPress)), mKeyRelease(getFunc(keyRelease)), mMouseClick(getFunc(mouseClick)),
            mMouseRelease(getFunc(mouseRelease)), mMouseMove(getFunc(mouseMove)), mContext(context), mModifiers(0)
            {
                assert(!instance);
                instance = this;

                initfreeablo_input();
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
				
				case SDL_WINDOWEVENT:
				{
					if(event.window.event == SDL_WINDOWEVENT_RESIZED)
						Render::resize(event.window.data1, event.window.data2);

					break;
				}

                default:
                {
                    break;
                }
            }

            while(!mQueue.push(e)) {} // push, or wait until buffer not full, then push
        }

        uint32_t mods = 0;

        SDL_Keymod sdlMods = SDL_GetModState();


        if(sdlMods & KMOD_CTRL)
            mods |= FAMOD_CTRL;

        if(sdlMods & KMOD_SHIFT)
            mods |= FAMOD_SHIFT;

        if(sdlMods & KMOD_ALT)
            mods |= FAMOD_ALT;

        mModifiers = mods;
    }
    
    int rocketTranslateMouse(Key key)
    {
        switch(key)
        {
            case KEY_LEFT_MOUSE:
                return 0;
            case KEY_RIGHT_MOUSE:
                return 1;
            case KEY_MIDDLE_MOUSE:
                return 2;
            default:
                return 3;
        }
    }

    int getRocketModifiers(uint32_t mods)
    {
        int retval = 0;

        if(mods & FAMOD_CTRL)
            retval |= Rocket::Core::Input::KM_CTRL;

        if(mods & FAMOD_SHIFT)
            retval |= Rocket::Core::Input::KM_SHIFT;

        if(mods & FAMOD_ALT)
            retval |= Rocket::Core::Input::KM_ALT;

        return retval;
    }

    int32_t consoleTranslateKey(int32_t key, uint32_t mods)
    {
        static const int32_t ASCII_OFFSET = 32;

        if(mods & FAMOD_SHIFT)
        {
            switch(key)
            {
                case SDLK_0: key = SDLK_RIGHTPAREN; break;
                case SDLK_1: key = SDLK_EXCLAIM; break;
                case SDLK_2: key = SDLK_AT; break;
                case SDLK_3: key = SDLK_HASH; break;
                case SDLK_4: key = SDLK_DOLLAR; break;
                case SDLK_5: key = SDLK_PERCENT; break;
                case SDLK_6: key = SDLK_CARET; break;
                case SDLK_7: key = SDLK_AMPERSAND; break;
                case SDLK_8: key = SDLK_ASTERISK; break;
                case SDLK_9: key = SDLK_LEFTPAREN; break;
                case SDLK_MINUS: key = SDLK_UNDERSCORE; break;
                case SDLK_EQUALS: key = SDLK_PLUS; break;
                case SDLK_SEMICOLON: key = SDLK_COLON; break;
                case SDLK_COMMA: key = SDLK_LESS; break;
                case SDLK_STOP: key = SDLK_GREATER; break;
                case SDLK_QUOTE: key = SDLK_QUOTEDBL; break;
                case SDLK_SLASH: key = SDLK_QUESTION; break;
                default: break;
            }

            if(key >= SDLK_a && key <= SDLK_z)
                key -= ASCII_OFFSET;
        }

        return key;
    }

    Rocket::Core::Input::KeyIdentifier rocketTranslateKey(int sdlkey)
    {
        using namespace Rocket::Core::Input;


        switch(sdlkey) {
            case SDLK_UNKNOWN:
                return KI_UNKNOWN;
                break;
            case SDLK_SPACE:
                return KI_SPACE;
                break;
            case SDLK_0:
                return KI_0;
                break;
            case SDLK_1:
                return KI_1;
                break;
            case SDLK_2:
                return KI_2;
                break;
            case SDLK_3:
                return KI_3;
                break;
            case SDLK_4:
                return KI_4;
                break;
            case SDLK_5:
                return KI_5;
                break;
            case SDLK_6:
                return KI_6;
                break;
            case SDLK_7:
                return KI_7;
                break;
            case SDLK_8:
                return KI_8;
                break;
            case SDLK_9:
                return KI_9;
                break;
            case SDLK_a:
                return KI_A;
                break;
            case SDLK_b:
                return KI_B;
                break;
            case SDLK_c:
                return KI_C;
                break;
            case SDLK_d:
                return KI_D;
                break;
            case SDLK_e:
                return KI_E;
                break;
            case SDLK_f:
                return KI_F;
                break;
            case SDLK_g:
                return KI_G;
                break;
            case SDLK_h:
                return KI_H;
                break;
            case SDLK_i:
                return KI_I;
                break;
            case SDLK_j:
                return KI_J;
                break;
            case SDLK_k:
                return KI_K;
                break;
            case SDLK_l:
                return KI_L;
                break;
            case SDLK_m:
                return KI_M;
                break;
            case SDLK_n:
                return KI_N;
                break;
            case SDLK_o:
                return KI_O;
                break;
            case SDLK_p:
                return KI_P;
                break;
            case SDLK_q:
                return KI_Q;
                break;
            case SDLK_r:
                return KI_R;
                break;
            case SDLK_s:
                return KI_S;
                break;
            case SDLK_t:
                return KI_T;
                break;
            case SDLK_u:
                return KI_U;
                break;
            case SDLK_v:
                return KI_V;
                break;
            case SDLK_w:
                return KI_W;
                break;
            case SDLK_x:
                return KI_X;
                break;
            case SDLK_y:
                return KI_Y;
                break;
            case SDLK_z:
                return KI_Z;
                break;
            case SDLK_SEMICOLON:
                return KI_OEM_1;
                break;
            case SDLK_PLUS:
                return KI_OEM_PLUS;
                break;
            case SDLK_COMMA:
                return KI_OEM_COMMA;
                break;
            case SDLK_MINUS:
                return KI_OEM_MINUS;
                break;
            case SDLK_PERIOD:
                return KI_OEM_PERIOD;
                break;
            case SDLK_SLASH:
                return KI_OEM_2;
                break;
            case SDLK_BACKQUOTE:
                return KI_OEM_3;
                break;
            case SDLK_LEFTBRACKET:
                return KI_OEM_4;
                break;
            case SDLK_BACKSLASH:
                return KI_OEM_5;
                break;
            case SDLK_RIGHTBRACKET:
                return KI_OEM_6;
                break;
            case SDLK_QUOTEDBL:
                return KI_OEM_7;
                break;
            case SDLK_KP_0:
                return KI_NUMPAD0;
                break;
            case SDLK_KP_1:
                return KI_NUMPAD1;
                break;
            case SDLK_KP_2:
                return KI_NUMPAD2;
                break;
            case SDLK_KP_3:
                return KI_NUMPAD3;
                break;
            case SDLK_KP_4:
                return KI_NUMPAD4;
                break;
            case SDLK_KP_5:
                return KI_NUMPAD5;
                break;
            case SDLK_KP_6:
                return KI_NUMPAD6;
                break;
            case SDLK_KP_7:
                return KI_NUMPAD7;
                break;
            case SDLK_KP_8:
                return KI_NUMPAD8;
                break;
            case SDLK_KP_9:
                return KI_NUMPAD9;
                break;
            case SDLK_KP_ENTER:
                return KI_NUMPADENTER;
                break;
            case SDLK_KP_MULTIPLY:
                return KI_MULTIPLY;
                break;
            case SDLK_KP_PLUS:
                return KI_ADD;
                break;
            case SDLK_KP_MINUS:
                return KI_SUBTRACT;
                break;
            case SDLK_KP_PERIOD:
                return KI_DECIMAL;
                break;
            case SDLK_KP_DIVIDE:
                return KI_DIVIDE;
                break;
            case SDLK_KP_EQUALS:
                return KI_OEM_NEC_EQUAL;
                break;
            case SDLK_BACKSPACE:
                return KI_BACK;
                break;
            case SDLK_TAB:
                return KI_TAB;
                break;
            case SDLK_CLEAR:
                return KI_CLEAR;
                break;
            case SDLK_RETURN:
                return KI_RETURN;
                break;
            case SDLK_PAUSE:
                return KI_PAUSE;
                break;
            case SDLK_CAPSLOCK:
                return KI_CAPITAL;
                break;
            case SDLK_PAGEUP:
                return KI_PRIOR;
                break;
            case SDLK_PAGEDOWN:
                return KI_NEXT;
                break;
            case SDLK_END:
                return KI_END;
                break;
            case SDLK_HOME:
                return KI_HOME;
                break;
            case SDLK_LEFT:
                return KI_LEFT;
                break;
            case SDLK_UP:
                return KI_UP;
                break;
            case SDLK_RIGHT:
                return KI_RIGHT;
                break;
            case SDLK_DOWN:
                return KI_DOWN;
                break;
            case SDLK_INSERT:
                return KI_INSERT;
                break;
            case SDLK_DELETE:
                return KI_DELETE;
                break;
            case SDLK_HELP:
                return KI_HELP;
                break;
            case SDLK_F1:
                return KI_F1;
                break;
            case SDLK_F2:
                return KI_F2;
                break;
            case SDLK_F3:
                return KI_F3;
                break;
            case SDLK_F4:
                return KI_F4;
                break;
            case SDLK_F5:
                return KI_F5;
                break;
            case SDLK_F6:
                return KI_F6;
                break;
            case SDLK_F7:
                return KI_F7;
                break;
            case SDLK_F8:
                return KI_F8;
                break;
            case SDLK_F9:
                return KI_F9;
                break;
            case SDLK_F10:
                return KI_F10;
                break;
            case SDLK_F11:
                return KI_F11;
                break;
            case SDLK_F12:
                return KI_F12;
                break;
            case SDLK_F13:
                return KI_F13;
                break;
            case SDLK_F14:
                return KI_F14;
                break;
            case SDLK_F15:
                return KI_F15;
                break;
            case SDLK_NUMLOCKCLEAR:
                return KI_NUMLOCK;
                break;
            case SDLK_SCROLLLOCK:
                return KI_SCROLL;
                break;
            case SDLK_LSHIFT:
                return KI_LSHIFT;
                break;
            case SDLK_RSHIFT:
                return KI_RSHIFT;
                break;
            case SDLK_LCTRL:
                return KI_LCONTROL;
                break;
            case SDLK_RCTRL:
                return KI_RCONTROL;
                break;
            case SDLK_LALT:
                return KI_LMENU;
                break;
            case SDLK_RALT:
                return KI_RMENU;
                break;
            case SDLK_LGUI:
                return KI_LMETA;
                break;
            case SDLK_RGUI:
                return KI_RMETA;
                break;
            /*case SDLK_LSUPER:
                return KI_LWIN;
                break;
            case SDLK_RSUPER:
                return KI_RWIN;
                break;*/
            case SDLK_ESCAPE:
                return KI_ESCAPE;
                break;
            default:
                return KI_UNKNOWN;
                break;
        }
    }

    uint32_t InputManager::getModifiers()
    {
        return mModifiers;
    }

    void InputManager::rocketBaseClicked()
    {
        mBaseWasClicked = true;
    }

    void InputManager::processInput(bool paused)
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
                    {
                        if(!paused)
                            mKeyPress(key);

                        if(mContext)
                        {
                            if(event.vals.key >= 32 && event.vals.key <= 255)
                            {
                                // How to convert properly event.vals.key + mModifier to lower/uppercase/chars like !@#$%^...?
                                int32_t consoleKey = consoleTranslateKey(event.vals.key, mModifiers);
                                mContext->ProcessTextInput(consoleKey);
                            }

                            mContext->ProcessKeyDown(rocketTranslateKey(event.vals.key), getRocketModifiers(mModifiers));

                            // Hack for moving cursor on the end of the text in input in librocket
                            // Default behaviour sets cursor on the start of input
                            if(event.vals.key == SDLK_UP || event.vals.key == SDLK_TAB)
                            {
                                mContext->ProcessKeyDown(rocketTranslateKey(SDLK_END), getRocketModifiers(mModifiers));
                            }
                        }
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
                        if(mContext)
                            mContext->ProcessKeyUp(rocketTranslateKey(event.vals.key), getRocketModifiers(mModifiers));
                    }
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    Key key = getMouseKey(event.vals.mouseButton.key);

                    if(key != KEY_UNDEF)
                    {
                        if(mContext)
                        {
                            mBaseWasClicked = false;
                            mContext->ProcessMouseButtonDown(rocketTranslateMouse(key), getRocketModifiers(mModifiers));

                            if(mBaseWasClicked && !paused)
                                mMouseClick(event.vals.mouseButton.x, event.vals.mouseButton.y, key);
                        }
                        else
                        {
                            if(!paused)
                                mMouseClick(event.vals.mouseButton.x, event.vals.mouseButton.y, key);
                        }
                    }
                    
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    Key key = getMouseKey(event.vals.mouseButton.key);

                    if(key != KEY_UNDEF)
                    {
                        if(!paused)
                            mMouseRelease(event.vals.mouseButton.x, event.vals.mouseButton.y, key);
                        if(mContext)
                            mContext->ProcessMouseButtonUp(rocketTranslateMouse(key), getRocketModifiers(mModifiers));
                    }
                    
                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    if(!paused)
                        mMouseMove(event.vals.mouseMove.x, event.vals.mouseMove.y);
                    if(mContext)
                        mContext->ProcessMouseMove(event.vals.mouseMove.x, event.vals.mouseMove.y, getRocketModifiers(mModifiers));
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
