// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2019, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#include "TestBase.h"

struct IssueTests : TestBase
{
};

struct AbstractClass
{
  virtual int sum (int a, int b) = 0;
};

struct ConcreteClass : AbstractClass
{
  int sum (int a, int b) override
  {
    return a + b;
  }

  static AbstractClass& get ()
  {
    static ConcreteClass instance;
    return instance;
  }
};

TEST_F (IssueTests, Issue87)
{
  luabridge::getGlobalNamespace (L)
    .beginClass <AbstractClass> ("Class")
    .addFunction ("sum", &AbstractClass::sum)
    .endClass ()
    .addFunction ("getAbstractClass", &ConcreteClass::get);

  runLua ("result = getAbstractClass ():sum (1, 2)");
  ASSERT_TRUE (result ().isNumber ());
  ASSERT_EQ (3, result <int> ());
}

TEST_F (IssueTests, Issue121)
{
  runLua (R"(
    first = {
      second = {
        actual = "data"
      }
    }
  )");
  auto first = luabridge::getGlobal (L, "first");
  ASSERT_TRUE (first.isTable ());
  ASSERT_EQ (0, first.length ());
  ASSERT_TRUE (first ["second"].isTable ());
  ASSERT_EQ (0, first ["second"].length ());
}

void pushArgs (lua_State*)
{
}

template <class Arg, class... Args>
void pushArgs (lua_State* L, Arg arg, Args... args)
{
  luabridge::Stack <Arg>::push (L, arg);
  pushArgs (L, args...);
}

template <class... Args>
std::vector <luabridge::LuaRef> callFunction (const luabridge::LuaRef& function, Args... args)
{
  assert (function.isFunction ());

  lua_State* L = function.state ();
  int originalTop = lua_gettop (L);
  function.push (L);
  pushArgs (L, args...);

  luabridge::LuaException::pcall (L, sizeof... (args), LUA_MULTRET);

  std::vector <luabridge::LuaRef> results;
  int top = lua_gettop (L);
  results.reserve (top - originalTop);
  for (int i = originalTop + 1; i <= top; ++i)
  {
    results.push_back (luabridge::LuaRef::fromStack (L, i));
  }
  return results;
}

TEST_F (IssueTests, Issue160)
{
  runLua (
    "function isConnected (arg1, arg2) "
    " return 1, 'srvname', 'ip:10.0.0.1', arg1, arg2 "
    "end");

  luabridge::LuaRef f_isConnected = luabridge::getGlobal (L, "isConnected");

  auto v = callFunction (f_isConnected, 2, "abc");
  ASSERT_EQ (5u, v.size ());
  ASSERT_EQ (1, v [0].cast <int> ());
  ASSERT_EQ ("srvname", v [1].cast <std::string> ());
  ASSERT_EQ ("ip:10.0.0.1", v [2].cast <std::string> ());
  ASSERT_EQ (2, v [3].cast <int> ());
  ASSERT_EQ ("abc", v [4].cast <std::string> ());
}

struct Vector
{
  float getX () const
  {
    return x;
  }

  float x = 0;
};

struct WideVector : Vector
{
  WideVector (float, float, float, float w)
  {
    x = w;
  }
};

TEST_F (IssueTests, Issue178)
{
  luabridge::getGlobalNamespace (L)
    .beginClass <Vector> ("Vector")
    .addFunction ("getX", &Vector::getX)
    .addProperty ("X", &Vector::getX)
    .addData ("x", &Vector::x, true)
    .endClass ()
    .deriveClass <WideVector, Vector> ("WideVector")
    .addConstructor <void (*) (float, float, float, float)> ()
    .endClass ();

  runLua ("result = WideVector (0, 1, 2, 3).x");

  ASSERT_TRUE (result ().isNumber ());
  ASSERT_EQ (3.f, result <float> ());
}

enum class MyEnum
{
  VALUE0,
  VALUE1,
};

template <typename T>
struct EnumWrapper
{
  static typename std::enable_if<std::is_enum<T>::value, void>::type push(lua_State* L, T value)
  {
    lua_pushnumber (L, static_cast<std::size_t> (value));
  }

  static typename std::enable_if<std::is_enum<T>::value, T>::type get(lua_State* L, int index)
  {
    return static_cast <T> (lua_tointeger (L, index));
  }
};

namespace luabridge {

template <>
struct Stack <MyEnum> : EnumWrapper <MyEnum>
{
};

} // namespace luabridge

TEST_F (IssueTests, Issue127)
{
  runLua ("result = 1");
  ASSERT_EQ (MyEnum::VALUE1, result <MyEnum> ());
}
