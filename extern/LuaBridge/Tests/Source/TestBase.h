// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2019, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// Copyright 2007, Nathan Reed
// SPDX-License-Identifier: MIT

#pragma once

#include "Lua/LuaLibrary.h"

#include "LuaBridge/LuaBridge.h"

#include <gtest/gtest.h>

#include <stdexcept>


// traceback function, adapted from lua.c
// when a runtime error occurs, this will append the call stack to the error message
//
inline int traceback (lua_State* L)
{
  // look up Lua's 'debug.traceback' function
  lua_getglobal (L, "debug");
  if (!lua_istable (L, -1))
  {
    lua_pop (L, 1);
    return 1;
  }
  lua_getfield (L, -1, "traceback");
  if (!lua_isfunction (L, -1))
  {
    lua_pop (L, 2);
    return 1;
  }
  lua_pushvalue (L, 1);  /* pass error message */
  lua_pushinteger (L, 2);  /* skip this function and traceback */
  lua_call (L, 2, 1);  /* call debug.traceback */
  return 1;
}

/// Base test class. Introduces the global 'result' variable,
/// used for checking of C++ - Lua interoperation.
///
struct TestBase : public ::testing::Test
{
  lua_State* L = nullptr;

  void SetUp () override
  {
    L = nullptr;
    L = luaL_newstate ();
    luaL_openlibs (L);
    lua_pushcfunction (L, &traceback);
  }

  void TearDown () override
  {
    if (L != nullptr)
    {
      lua_close (L);
    }
  }

  void runLua (const std::string& script) const
  {
    if (luaL_loadstring (L, script.c_str ()) != 0)
    {
      throw std::runtime_error (lua_tostring (L, -1));
    }

    if (lua_pcall (L, 0, 0, -2) != 0)
    {
      throw std::runtime_error (lua_tostring (L, -1));
    }
  }

  template <class T = luabridge::LuaRef>
  T result () const
  {
    return luabridge::getGlobal (L, "result").cast <T> ();
  }

  void resetResult () const
  {
    luabridge::setGlobal (L, luabridge::LuaRef (L), "result");
  }

  void printStack () const
  {
    std::cerr << "===== Stack =====\n";
    for (int i = 1; i <= lua_gettop (L); ++i)
    {
      std::cerr << "@" << i << " = " << luabridge::LuaRef::fromStack (L, i) << "\n";
    }
  }
};
