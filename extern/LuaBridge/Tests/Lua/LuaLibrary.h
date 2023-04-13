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

/** Include file for the Lua library.
*/

#pragma once

// This determines which version of Lua to use.
// The value is the same as LUA_VERSION_NUM in lua.h

#ifndef LUABRIDGEDEMO_LUA_VERSION
#if 1
  #define LUABRIDGEDEMO_LUA_VERSION 502   // use 5.2.0
#else
  #define LUABRIDGEDEMO_LUA_VERSION 501   // use 5.1.0 (really 5.1.5)
#endif
#endif

#ifndef LUALIBRARY_SOURCE

#if LUABRIDGEDEMO_LUA_VERSION >= 502
  #include "Lua.5.2.0/src/lua.hpp"

#elif LUABRIDGEDEMO_LUA_VERSION >= 501
extern "C"
{
#include "Lua.5.1.5/src/lua.h"
#include "Lua.5.1.5/src/lualib.h"
#include "Lua.5.1.5/src/lauxlib.h"
}

#else
  #error "Unknown LUA_VERSION_NUM"

#endif

#endif
