#include <Rocket/Core/Input.h>
#include <boost/python.hpp>
#include <cctype>
#include "keys.h"
//#include "common.h"

namespace rci = Rocket::Core::Input;

/* namespace Input
{
    
    Key convertRocketKey(int rocketk)
    {
        Key key;
        
        switch(rocketk) 
        {
            case rci::KI_BACK: key = KEY_BACKSPACE; break;
            case rci::KI_TAB: key = KEY_TAB; break;
            case rci::KI_CLEAR: key = KEY_CLEAR; break;
            case rci::KI_RETURN: key = KEY_RETURN; break;
            case rci::KI_PAUSE: key = KEY_PAUSE; break;
            case rci::KI_ESCAPE: key = KEY_ESCAPE; break;
            case rci::KI_SPACE: key = KEY_SPACE; break;
            case rci::KI_OEM_8: KEY_QUOTEDBL
            case rci::KEY_HASH KEY_HASH
            case rci::KEY_DOLLAR KEY_DOLLAR 
            case rci::KEY_AMPERSAND KEY_AMPERSAND
            case rci::KEY_QUOTE KEY_QUOTE
            case rci::KEY_LEFTPAREN KEY_LEFTPAREN
            case rci::KEY_RIGHTPAREN KEY_RIGHTPAREN
            case rci::KEY_ASTERISK KEY_ASTERISK
                    
            case rci::KI_SPACE: key = KEY_SPACE; break;
            case rci::KI_0: key = KEY_0; break;
            case rci::KI_1: key = KEY_1; break;
            case rci::KI_2: key = KEY_2; break;
            case rci::KI_3: key = KEY_3; break;
            case rci::KI_4: key = KEY_4; break;
            case rci::KI_5: key = KEY_5; break;
            case rci::KI_6: key = KEY_6; break;
            case rci::KI_7: key = KEY_7; break;
            case rci::KI_8: key = KEY_8; break;
            case rci::KI_9: key = KEY_9; break;
            
            case rci::KI_A: key = KEY_a; break;
            case rci::KI_B: key = KEY_b; break;
            case rci::KI_C: key = KEY_c; break;
            case rci::KI_D: key = KEY_d; break;
            case rci::KI_E: key = KEY_e; break;
            case rci::KI_F: key = KEY_f; break;
            case rci::KI_G: key = KEY_g; break;
            case rci::KI_H: key = KEY_h; break;
            case rci::KI_I: key = KEY_i; break;
            case rci::KI_J: key = KEY_j; break;
            case rci::KI_K: key = KEY_k; break;
            case rci::KI_L: key = KEY_l; break;
            case rci::KI_M: key = KEY_m; break;
            case rci::KI_N: key = KEY_n; break;
            case rci::KI_O: key = KEY_o; break;
            case rci::KI_P: key = KEY_p; break;
            case rci::KI_Q: key = KEY_q; break;
            case rci::KI_R: key = KEY_r; break;
            case rci::KI_S: key = KEY_s; break;
            case rci::KI_T: key = KEY_t; break;
            case rci::KI_U: key = KEY_u; break;
            case rci::KI_V: key = KEY_v; break;
            case rci::KI_W: key = KEY_w; break;
            case rci::KI_X: key = KEY_x; break;
            case rci::KI_Y: key = KEY_y; break;
            case rci::KI_Z: key = KEY_z; break;
            
            case rci::KI_OEM_1: key = KEY_SEMICOLON
            case rci::KI_OEM_PLUS: key = KEY_PLUS
            case rci::KI_OEM_COMMA: key = KEY_COMMA 
            case rci::KI_OEM_MINUS: key = KEY_MINUS 
            case rci::KI_OEM_PERIODK: key = KEY_PERIOD
            case rci::KI_OEM_2: key = KEY_QUESTION
            case rci::KI_OEM_3: key = 	

            case rci::KI_OEM_4: key =  
            case rci::KI_OEM_5: key =  
            case rci::KI_OEM_6: key =  		
            case rci::KI_OEM_7: key =  
            case rci::KI_OEM_8: key =  

            case rci::KI_OEM_102: key = 	
: key = 
            case rci::KI_NUMPAD0: key = 
            case rci::KI_NUMPAD1: key = 
            case rci::KI_NUMPAD2: key = 
            case rci::KI_NUMPAD3: key = 
            case rci::KI_NUMPAD4: key = 
            case rci::KI_NUMPAD5: key = 
            case rci::KI_NUMPAD6: key = 
            case rci::KI_NUMPAD7: key = 
            case rci::KI_NUMPAD8: key = 
            case rci::KI_NUMPAD9: key = 
            case rci::KI_NUMPADENTER: key = 
            case rci::KI_MULTIPLY: key = 
            case rci::KI_ADD: key = 		
            case rci::KI_SEPARATOR: key = 
            case rci::KI_SUBTRACT: key = 
            case rci::KI_DECIMAL: key = 
            case rci::KI_DIVIDE: key = 	

            case rci::KI_OEM_NEC_EQUAL: key = 		

            case rci::KI_BACK: key = 			
            case rci::KI_TAB: key = 			

            case rci::KI_CLEAR: key = 
            case rci::KI_RETURN: key = 

            case rci::KI_PAUSE: key = 
            case rci::KI_CAPITAL: key = 			

            case rci::KI_KANA = 75,				
            case rci::KI_HANGUL = 76,			
            case rci::KI_JUNJA = 77,			
            case rci::KI_FINAL = 78,			
            case rci::KI_HANJA = 79,			
            case rci::KI_KANJI = 80,			

            case rci::KI_ESCAPE = 81,			

            case rci::KI_CONVERT = 82,			
            case rci::KI_NONCONVERT = 83,			
            case rci::KI_ACCEPT = 84,			
            case rci::KI_MODECHANGE = 85,			

            case rci::KI_PRIOR = 86,			
            case rci::KI_NEXT = 87,				
            case rci::KI_END = 88,
            case rci::KI_HOME = 89,
            case rci::KI_LEFT = 90,				
            case rci::KI_UP = 91,				
            case rci::KI_RIGHT = 92,			
            case rci::KI_DOWN = 93,				
            case rci::KI_SELECT = 94,
            case rci::KI_PRINT = 95,
            case rci::KI_EXECUTE = 96,
            case rci::KI_SNAPSHOT = 97,			
            case rci::KI_INSERT = 98,
            case rci::KI_DELETE = 99,
            case rci::KI_HELP = 100,

            case rci::KI_LWIN = 101,			
            case rci::KI_RWIN = 102,			
            case rci::KI_APPS = 103,			

            case rci::KI_POWER = 104,
            case rci::KI_SLEEP = 105,
            case rci::KI_WAKE = 106,

            case rci::KI_F1 = 107,
            case rci::KI_F2 = 108,
            case rci::KI_F3 = 109,
            case rci::KI_F4 = 110,
            case rci::KI_F5 = 111,
            case rci::KI_F6 = 112,
            case rci::KI_F7 = 113,
            case rci::KI_F8 = 114,
            case rci::KI_F9 = 115,
            case rci::KI_F10 = 116,
            case rci::KI_F11 = 117,
            case rci::KI_F12 = 118,
            case rci::KI_F13 = 119,
            case rci::KI_F14 = 120,
            case rci::KI_F15 = 121,
            case rci::KI_F16 = 122,
            case rci::KI_F17 = 123,
            case rci::KI_F18 = 124,
            case rci::KI_F19 = 125,
            case rci::KI_F20 = 126,
            case rci::KI_F21 = 127,
            case rci::KI_F22 = 128,
            case rci::KI_F23 = 129,
            case rci::KI_F24 = 130,

            case rci::KI_NUMLOCK = 131,		
            case rci::KI_SCROLL = 132,		

            case rci::KI_OEM_FJ_JISHO = 133,	
            case rci::KI_OEM_FJ_MASSHOU = 134,	
            case rci::KI_OEM_FJ_TOUROKU = 135,	
            case rci::KI_OEM_FJ_LOYA = 136,		
            case rci::KI_OEM_FJ_ROYA = 137,		

            case rci::KI_LSHIFT = 138,
            case rci::KI_RSHIFT = 139,
            case rci::KI_LCONTROL = 140,
            case rci::KI_RCONTROL = 141,
            case rci::KI_LMENU = 142,
            case rci::KI_RMENU = 143,

            case rci::KI_BROWSER_BACK = 144,
            case rci::KI_BROWSER_FORWARD = 145,
            case rci::KI_BROWSER_REFRESH = 146,
            case rci::KI_BROWSER_STOP = 147,
            case rci::KI_BROWSER_SEARCH = 148,
            case rci::KI_BROWSER_FAVORITES = 149,
            case rci::KI_BROWSER_HOME = 150,

            case rci::KI_VOLUME_MUTE = 151,
            case rci::KI_VOLUME_DOWN = 152,
            case rci::KI_VOLUME_UP = 153,
            case rci::KI_MEDIA_NEXT_TRACK = 154,
            case rci::KI_MEDIA_PREV_TRACK = 155,
            case rci::KI_MEDIA_STOP = 156,
            case rci::KI_MEDIA_PLAY_PAUSE = 157,
            case rci::KI_LAUNCH_MAIL = 158,
            case rci::KI_LAUNCH_MEDIA_SELECT = 159,
            case rci::KI_LAUNCH_APP1 = 160,
            case rci::KI_LAUNCH_APP2 = 161,

            case rci::KI_OEM_AX = 162,
            case rci::KI_ICO_HELP = 163,
            case rci::KI_ICO_00 = 164,

            case rci::KI_PROCESSKEY = 165,		

            case rci::KI_ICO_CLEAR = 166,

            case rci::KI_ATTN = 167,
            case rci::KI_CRSEL = 168,
            case rci::KI_EXSEL = 169,
            case rci::KI_EREOF = 170,
            case rci::KI_PLAY = 171,
            case rci::KI_ZOOM = 172,
            case rci::KI_PA1 = 173,
            case rci::KI_OEM_CLEAR = 174,

            case rci::KI_LMETA = 175,
            case rci::KI_RMETA = 176
            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }
    }
}*/

namespace Input
{
    #define RTACASE2(val1, val2) case rci::KI_##val1: key = KEY_##val2; break
    #define RTACASE(val) case rci::KI_##val: key = KEY_##val; break;

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

            RTACASE2(RMETA, RSUPER);
            RTACASE2(LMETA, LSUPER);
         
            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }

        return key;

    }
    
    #define ATRCASE2(val1, val2) case KEY_##val1: key = rci::KI_##val2; break;
    #define ATRCASE(val) case KEY_##val: key = rci::KI_##val; break;
    
    int convertAsciiToRocketKey(int rocketk)
    {
        int key;
        
        switch(rocketk)
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

            ATRCASE2(RSUPER, RMETA);
            ATRCASE2(LSUPER, LMETA);
         
            default:
            {
                key = KEY_UNDEF;
                break;
            }
        }

        return key;

    }
    
    std::string getHotkeyName(int key [])
    {
        std::string hotkeyname;
        if (key[1])
        {
            hotkeyname += "SHIFT + ";
        }
        if (key[2])
        {
            hotkeyname += "CTRL + ";
        }
        if (key[3])
        {
            hotkeyname += "ALT + ";
        }
        if (key[4])
        {
            hotkeyname += "META + ";
        }
        
        switch(key[0])
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
               hotkeyname += std::toupper(char(key[0])); 
            }
        }
        
        return hotkeyname;
    }
}
