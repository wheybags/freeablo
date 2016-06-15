#ifndef COMMON_H
#define	COMMON_H

#include "keys.h"
#include "hotkey.h"

namespace rci = Rocket::Core::Input;

namespace Input
{
    #define RTACASE2(val1, val2) case rci::KI_##val1: key = KEY_##val2; break
    #define RTACASE(val) case rci::KI_##val: key = KEY_##val; break;
    
    Key convertRocketKeyToAscii(int rocketk);
    
    #define ATRCASE2(val1, val2) case KEY_##val1: key = rci::KI_##val2; break;
    #define ATRCASE(val) case KEY_##val: key = rci::KI_##val; break;

    int convertAsciiToRocketKey(int asciik);
}

#endif	/* COMMON_H */

