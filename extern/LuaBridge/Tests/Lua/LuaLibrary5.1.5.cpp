//==============================================================================
/*
  https://github.com/vinniefalco/LuaBridge
  https://github.com/vinniefalco/LuaBridgeDemo
  
  Copyright (C) 2012, Vinnie Falco <vinnie.falco@gmail.com>

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#include "JuceLibraryCode/AppConfig.h"

#define LUALIBRARY_SOURCE
#include "LuaLibrary.h"

#if LUABRIDGEDEMO_LUA_VERSION == 501

#if _MSC_VER
#pragma push_macro("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* luaconf.h only declares some things if it is being included
   from certain source files. We define all the relevant macros
   and include luaconf.h once so we get all the declarations.
*/
#define LUA_CORE
#define loslib_c
#include "Lua.5.1.5/src/luaconf.h"
#undef LUA_CORE
#undef loslib_c

#if _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4244) /* Possible loss of data */
#pragma warning (disable: 4702) /* Unreachable code */
#endif

/* Include this early to prevent the conflict with luai_hashnum
   and supress the warning caused by #define lua_assert
*/
#include "Lua.5.1.5/src/lapi.c"
#include "Lua.5.1.5/src/lauxlib.c"
#include "Lua.5.1.5/src/lbaselib.c"
#include "Lua.5.1.5/src/lcode.c"
#include "Lua.5.1.5/src/ldblib.c"
#include "Lua.5.1.5/src/ldebug.c"
#include "Lua.5.1.5/src/ldo.c"
#include "Lua.5.1.5/src/ldump.c"
#include "Lua.5.1.5/src/lfunc.c"
#include "Lua.5.1.5/src/lgc.c"
#include "Lua.5.1.5/src/linit.c"
#include "Lua.5.1.5/src/liolib.c"
#include "Lua.5.1.5/src/llex.c"
#include "Lua.5.1.5/src/lmathlib.c"
#include "Lua.5.1.5/src/lmem.c"
#include "Lua.5.1.5/src/lobject.c"
#include "Lua.5.1.5/src/lopcodes.c"
#include "Lua.5.1.5/src/loslib.c"
#include "Lua.5.1.5/src/lparser.c"
#include "Lua.5.1.5/src/lstate.c"
#include "Lua.5.1.5/src/lstring.c"
#include "Lua.5.1.5/src/lstrlib.c"
#include "Lua.5.1.5/src/ltable.c"
#include "Lua.5.1.5/src/ltablib.c"
#include "Lua.5.1.5/src/ltm.c"
#include "Lua.5.1.5/src/lundump.c"
#include "Lua.5.1.5/src/lvm.c"
#include "Lua.5.1.5/src/lzio.c"
//#include "Lua.5.1.5/src/print.c"

/* loadlib.c includes Windows.h, which defines the LoadString macro,
   so include it last to prevent errors.
*/
#include "Lua.5.1.5/src/loadlib.c"

#if _MSC_VER
#pragma warning (pop)
#endif

#ifdef __cplusplus
}
#endif

#if _MSC_VER
#pragma pop_macro("_CRT_SECURE_NO_WARNINGS")
#endif

#endif
