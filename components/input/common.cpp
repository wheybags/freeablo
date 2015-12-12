#include <Rocket/Core/Input.h>
#include <misc/boost_python.h>
#include <cctype>
#include "common.h"

namespace Input
{
    Key convertRocketKeyToAscii(int rocketk)
    {
        Key key;
        
        switch(rocketk)
        {
            RTACASE2(BACK, BACKSPACE);
            RTACASE(TAB);
            RTACASE(CLEAR);
            RTACASE(RETURN);
            RTACASE(PAUSE);
            RTACASE(ESCAPE);
            RTACASE(SPACE);
            RTACASE2(OEM_7, QUOTEDBL);
            RTACASE2(OEM_PLUS, PLUS);
            RTACASE2(OEM_COMMA, COMMA);
            RTACASE2(OEM_MINUS, MINUS);
            RTACASE2(OEM_PERIOD, PERIOD);
            RTACASE2(OEM_2, SLASH);
            RTACASE(0);
            RTACASE(1);
            RTACASE(2);
            RTACASE(3);
            RTACASE(4);
            RTACASE(5);
            RTACASE(6);
            RTACASE(7);
            RTACASE(8);
            RTACASE(9);
            RTACASE2(OEM_1, SEMICOLON);
            RTACASE2(OEM_NEC_EQUAL, EQUALS);

            RTACASE2(OEM_4, LEFTBRACKET);
            RTACASE2(OEM_5, BACKSLASH);
            RTACASE2(OEM_6, RIGHTBRACKET);
            RTACASE2(OEM_3, BACKQUOTE);
            RTACASE2(A, a);
            RTACASE2(B, b);
            RTACASE2(C, c);
            RTACASE2(D, d);
            RTACASE2(E, e);
            RTACASE2(F, f);
            RTACASE2(G, g);
            RTACASE2(H, h);
            RTACASE2(I, i);
            RTACASE2(J, j);
            RTACASE2(K, k);
            RTACASE2(L, l);
            RTACASE2(M, m);
            RTACASE2(N, n);
            RTACASE2(O, o);
            RTACASE2(P, p);
            RTACASE2(Q, q);
            RTACASE2(R, r);
            RTACASE2(S, s);
            RTACASE2(T, t);
            RTACASE2(U, u);
            RTACASE2(V, v);
            RTACASE2(W, w);
            RTACASE2(X, x);
            RTACASE2(Y, y);
            RTACASE2(Z, z);
            RTACASE(DELETE);

            RTACASE2(DECIMAL, KP_PERIOD);
            RTACASE2(DIVIDE, KP_DIVIDE);
            RTACASE2(MULTIPLY, KP_MULTIPLY);
            RTACASE2(SUBTRACT, KP_MINUS);
            RTACASE2(ADD, KP_PLUS);
            RTACASE2(NUMPADENTER, KP_ENTER);

            RTACASE(UP);
            RTACASE(DOWN);
            RTACASE(RIGHT);
            RTACASE(LEFT);
            RTACASE(INSERT);
            RTACASE(HOME);
            RTACASE(END);
            RTACASE2(PRIOR, PAGEUP);
            RTACASE2(NEXT, PAGEDOWN);

            RTACASE(F1);
            RTACASE(F2);
            RTACASE(F3);
            RTACASE(F4);
            RTACASE(F5);
            RTACASE(F6);
            RTACASE(F7);
            RTACASE(F8);
            RTACASE(F9);
            RTACASE(F10);
            RTACASE(F11);
            RTACASE(F12);
            RTACASE(F13);
            RTACASE(F14);
            RTACASE(F15);

            RTACASE2(CAPITAL, CAPSLOCK);
            RTACASE(RSHIFT);
            RTACASE(LSHIFT);
            RTACASE2(RCONTROL, RCTRL);
            RTACASE2(LCONTROL, LCTRL);
            RTACASE2(RMENU, RALT);
            RTACASE2(LMENU, LALT);

            RTACASE2(NUMPAD0, KP0);
            RTACASE2(NUMPAD1, KP1);
            RTACASE2(NUMPAD2, KP2);
            RTACASE2(NUMPAD3, KP3);
            RTACASE2(NUMPAD4, KP4);
            RTACASE2(NUMPAD5, KP5);
            RTACASE2(NUMPAD6, KP6);
            RTACASE2(NUMPAD7, KP7);
            RTACASE2(NUMPAD8, KP8);
            RTACASE2(NUMPAD9, KP9);

            RTACASE(NUMLOCK);
            RTACASE2(SCROLL, SCROLLOCK);
         
            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }

        return key;

    }
    
    int convertAsciiToRocketKey(int asciik)
    {
        int key;
        
        switch(asciik)
        {
            ATRCASE2(BACKSPACE, BACK);
            ATRCASE(TAB);
            ATRCASE(CLEAR);
            ATRCASE(RETURN);
            ATRCASE(PAUSE);
            ATRCASE(ESCAPE);
            ATRCASE(SPACE);
            ATRCASE2(QUOTEDBL, OEM_7);
            ATRCASE2(PLUS, OEM_PLUS);
            ATRCASE2(COMMA, OEM_COMMA);
            ATRCASE2(MINUS, OEM_MINUS);
            ATRCASE2(PERIOD, OEM_PERIOD);
            ATRCASE2(SLASH, OEM_2);
            ATRCASE(0);
            ATRCASE(1);
            ATRCASE(2);
            ATRCASE(3);
            ATRCASE(4);
            ATRCASE(5);
            ATRCASE(6);
            ATRCASE(7);
            ATRCASE(8);
            ATRCASE(9);
            ATRCASE2(SEMICOLON, OEM_1);
            ATRCASE2(EQUALS, OEM_NEC_EQUAL);

            ATRCASE2(LEFTBRACKET, OEM_4);
            ATRCASE2(BACKSLASH, OEM_5);
            ATRCASE2(RIGHTBRACKET, OEM_6);
            ATRCASE2(BACKQUOTE, OEM_3);
            ATRCASE2(a, A);
            ATRCASE2(b, B);
            ATRCASE2(c, C);
            ATRCASE2(d, D);
            ATRCASE2(e, E);
            ATRCASE2(f, F);
            ATRCASE2(g, G);
            ATRCASE2(h, H);
            ATRCASE2(i, I);
            ATRCASE2(j, J);
            ATRCASE2(k, K);
            ATRCASE2(l, L);
            ATRCASE2(m, M);
            ATRCASE2(n, N);
            ATRCASE2(o, O);
            ATRCASE2(p, P);
            ATRCASE2(q, Q);
            ATRCASE2(r, R);
            ATRCASE2(s, S);
            ATRCASE2(t, T);
            ATRCASE2(u, U);
            ATRCASE2(v, V);
            ATRCASE2(w, W);
            ATRCASE2(x, X);
            ATRCASE2(y, Y);
            ATRCASE2(z, Z);
            ATRCASE(DELETE);

            ATRCASE2(KP_PERIOD, DECIMAL);
            ATRCASE2(KP_DIVIDE, DIVIDE);
            ATRCASE2(KP_MULTIPLY, MULTIPLY);
            ATRCASE2(KP_MINUS, SUBTRACT);
            ATRCASE2(KP_PLUS, ADD);
            ATRCASE2(KP_ENTER, NUMPADENTER);

            ATRCASE(UP);
            ATRCASE(DOWN);
            ATRCASE(RIGHT);
            ATRCASE(LEFT);
            ATRCASE(INSERT);
            ATRCASE(HOME);
            ATRCASE(END);
            ATRCASE2(PAGEUP, PRIOR);
            ATRCASE2(PAGEDOWN, NEXT);

            ATRCASE(F1);
            ATRCASE(F2);
            ATRCASE(F3);
            ATRCASE(F4);
            ATRCASE(F5);
            ATRCASE(F6);
            ATRCASE(F7);
            ATRCASE(F8);
            ATRCASE(F9);
            ATRCASE(F10);
            ATRCASE(F11);
            ATRCASE(F12);
            ATRCASE(F13);
            ATRCASE(F14);
            ATRCASE(F15);

            ATRCASE2(CAPSLOCK, CAPITAL);
            ATRCASE(RSHIFT);
            ATRCASE(LSHIFT);
            ATRCASE2(RCTRL, RCONTROL);
            ATRCASE2(LCTRL, LCONTROL);
            ATRCASE2(RALT, RMENU);
            ATRCASE2(LALT, LMENU);

            ATRCASE2(KP0, NUMPAD0);
            ATRCASE2(KP1, NUMPAD1);
            ATRCASE2(KP2, NUMPAD2);
            ATRCASE2(KP3, NUMPAD3);
            ATRCASE2(KP4, NUMPAD4);
            ATRCASE2(KP5, NUMPAD5);
            ATRCASE2(KP6, NUMPAD6);
            ATRCASE2(KP7, NUMPAD7);
            ATRCASE2(KP8, NUMPAD8);
            ATRCASE2(KP9, NUMPAD9);

            ATRCASE(NUMLOCK);
            ATRCASE2(SCROLLOCK, SCROLL);
         
            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }

        return key;

    }
    
    std::string getHotkeyName(Hotkey key)
    {
        std::string hotkeyname;
        if (key.shift)
        {
            hotkeyname += "SHIFT + ";
        }
        if (key.ctrl)
        {
            hotkeyname += "CTRL + ";
        }
        if (key.alt)
        {
            hotkeyname += "ALT + ";
        }
        
        switch(key.key)
        {
            case Input::KEY_F1: hotkeyname += "F1"; break;
            case Input::KEY_F2: hotkeyname += "F2"; break;
            case Input::KEY_F3: hotkeyname += "F3"; break;
            case Input::KEY_F4: hotkeyname += "F4"; break;
            case Input::KEY_F5: hotkeyname += "F5"; break;
            case Input::KEY_F6: hotkeyname += "F6"; break;
            case Input::KEY_F7: hotkeyname += "F7"; break;
            case Input::KEY_F8: hotkeyname += "F8"; break;
            case Input::KEY_F9: hotkeyname += "F9"; break;
            case Input::KEY_F10: hotkeyname += "F10"; break;
            case Input::KEY_F11: hotkeyname += "F11"; break;
            case Input::KEY_F12: hotkeyname += "F12"; break;
            case Input::KEY_F13: hotkeyname += "F13"; break;
            case Input::KEY_F14: hotkeyname += "F14"; break;
            case Input::KEY_F15: hotkeyname += "F15"; break;
            
            case Input::KEY_BACKSPACE: hotkeyname += "BACKSPACE"; break;
            case Input::KEY_TAB: hotkeyname += "TAB"; break;
            case Input::KEY_CLEAR: hotkeyname += "CLEAR"; break;
            case Input::KEY_SPACE: hotkeyname += "SPACE"; break;
            case Input::KEY_DELETE: hotkeyname += "DELETE"; break;
            
            case Input::KEY_KP0: hotkeyname += "KP0"; break;
            case Input::KEY_KP1: hotkeyname += "KP1"; break;
            case Input::KEY_KP2: hotkeyname += "KP2"; break;
            case Input::KEY_KP3: hotkeyname += "KP3"; break;
            case Input::KEY_KP4: hotkeyname += "KP4"; break;
            case Input::KEY_KP5: hotkeyname += "KP5"; break;
            case Input::KEY_KP6: hotkeyname += "KP6"; break;
            case Input::KEY_KP7: hotkeyname += "KP7"; break;
            case Input::KEY_KP8: hotkeyname += "KP8"; break;
            case Input::KEY_KP9: hotkeyname += "KP9"; break;
            case Input::KEY_KP_PERIOD: hotkeyname += "KP_PERIOD"; break;
            case Input::KEY_KP_DIVIDE: hotkeyname += "KP_DIVIDE"; break;
            case Input::KEY_KP_MULTIPLY: hotkeyname += "KP_MULTIPLY"; break;
            case Input::KEY_KP_MINUS: hotkeyname += "KP_MINUS"; break;
            case Input::KEY_KP_PLUS: hotkeyname += "KP_PLUS"; break;
            case Input::KEY_KP_EQUALS: hotkeyname += "KP_EQUALS"; break;
            
            case Input::KEY_UP: hotkeyname += "UP"; break;
            case Input::KEY_DOWN: hotkeyname += "DOWN"; break;
            case Input::KEY_RIGHT: hotkeyname += "RIGHT"; break;
            case Input::KEY_LEFT: hotkeyname += "LEFT"; break;
            case Input::KEY_INSERT: hotkeyname += "INSERT"; break;
            case Input::KEY_HOME: hotkeyname += "HOME"; break;
            case Input::KEY_END: hotkeyname += "END"; break;
            case Input::KEY_PAGEUP: hotkeyname += "PAGEUP"; break;
            case Input::KEY_PAGEDOWN: hotkeyname += "PAGEDOWN"; break;
            
            default:
            {
               hotkeyname += std::toupper(char(key.key)); 
            }
        }
        
        return hotkeyname;
    }
}
