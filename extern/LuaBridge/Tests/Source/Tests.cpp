// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2019, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// Copyright 2007, Nathan Reed
// SPDX-License-Identifier: MIT

// A set of tests of different types' communication with Lua

#include "TestBase.h"

#include <gtest/gtest.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

void printValue (lua_State* L, int index)
{
  int type = lua_type (L, index);
  switch (type)
  {
    case LUA_TBOOLEAN:
      std::cerr << std::boolalpha << (lua_toboolean (L, index) != 0);
      break;
    case LUA_TSTRING:
      std::cerr << lua_tostring (L, index);
      break;
    case LUA_TNUMBER:
      std::cerr << lua_tonumber (L, index);
      break;
    case LUA_TTABLE:
    case LUA_TTHREAD:
    case LUA_TFUNCTION:
      std::cerr << lua_topointer (L, index);
      break;
  }
  std::cerr << ": " << lua_typename (L, type) << " (" << type << ")" << std::endl;
}

struct LuaBridgeTest : TestBase
{
};

template <class T>
T identityCFunction(T value)
{
  return value;
}

TEST_F (LuaBridgeTest, CFunction)
{
  luabridge::getGlobalNamespace(L)
    .addFunction ("boolFn", &identityCFunction <bool>)
    .addFunction ("ucharFn", &identityCFunction <unsigned char>)
    .addFunction ("shortFn", &identityCFunction <short>)
    .addFunction ("ushortFn", &identityCFunction <unsigned short>)
    .addFunction ("intFn", &identityCFunction <int>)
    .addFunction ("uintFn", &identityCFunction <unsigned int>)
    .addFunction ("longFn", &identityCFunction <long>)
    .addFunction ("ulongFn", &identityCFunction <unsigned long>)
    .addFunction ("longlongFn", &identityCFunction <long long>)
    .addFunction ("ulonglongFn", &identityCFunction <unsigned long long>)
    .addFunction ("floatFn", &identityCFunction <float>)
    .addFunction ("doubleFn", &identityCFunction <double>)
    .addFunction ("charFn", &identityCFunction <char>)
    .addFunction ("cstringFn", &identityCFunction <const char*>)
    .addFunction ("stringFn", &identityCFunction <std::string>)
  ;

  {
    runLua ("result = ucharFn (255)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (255u, result <unsigned char> ());
  }

  {
    runLua ("result = boolFn (false)");
    ASSERT_EQ (true, result ().isBool ());
    ASSERT_EQ (false, result <bool> ());
  }
  {
    runLua ("result = boolFn (true)");
    ASSERT_EQ (true, result ().isBool ());
    ASSERT_EQ (true, result <bool> ());
  }

  {
    runLua ("result = shortFn (-32768)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (-32768, result <int> ());
  }

  {
    runLua ("result = ushortFn (32767)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (32767u, result <unsigned int> ());
  }
  {
    runLua ("result = intFn (-500)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (-500, result <int> ());
  }

  {
    runLua ("result = uintFn (42)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (42u, result <unsigned int> ());
  }

  {
    runLua ("result = longFn (-8000)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (-8000, result <long> ());
  }

  {
    runLua ("result = ulongFn (9000)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (9000u, result <unsigned long> ());
  }

  {
    runLua ("result = longlongFn (-8000)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (-8000, result <long long> ());
  }

  {
    runLua ("result = ulonglongFn (9000)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_EQ (9000u, result <unsigned long long> ());
  }

  {
    runLua ("result = floatFn (3.14)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_FLOAT_EQ (3.14f, result <float> ());
  }

  {
    runLua ("result = doubleFn (-12.3)");
    ASSERT_EQ (true, result ().isNumber ());
    ASSERT_DOUBLE_EQ (-12.3, result <double> ());
  }

  {
    runLua ("result = charFn ('a')");
    ASSERT_EQ (true, result ().isString ());
    ASSERT_EQ ('a', result <char> ());
  }

  {
    runLua ("result = cstringFn ('abc')");
    ASSERT_EQ (true, result ().isString ());
    ASSERT_STREQ ("abc", result <const char*> ());
  }

  {
    runLua ("result = stringFn ('lua')");
    ASSERT_EQ (true, result ().isString ());
    ASSERT_EQ ("lua", result <std::string> ());
  }
}

template <class T>
struct TestClass
{
  TestClass (T data)
    : data (data)
    , constData (data)
  {
  }

  T getValue () { return data; }
  T* getPtr () { return &data; }
  T const* getConstPtr () { return &data; }
  T& getRef () { return data; }
  T const& getConstRef () { return data; }
  T getValueConst () const { return data; }
  T* getPtrConst () const { return &data; }
  T const* getConstPtrConst () const { return &data; }
  T& getRefConst () const { return data; }
  T const& getConstRefConst () const { return data; }

  mutable T data;
  mutable T constData;
};

TEST_F (LuaBridgeTest, ClassFunction)
{
  typedef TestClass <int> Inner;
  typedef TestClass <Inner> Outer;

  luabridge::getGlobalNamespace (L)
    .beginClass <Inner> ("Inner")
    .addConstructor <void (*) (int)> ()
    .addData ("data", &Inner::data)
    .endClass ()
    .beginClass <Outer> ("Outer")
    .addConstructor <void (*) (Inner)> ()
    .addFunction ("getValue", &Outer::getValue)
    .addFunction ("getPtr", &Outer::getPtr)
    .addFunction ("getConstPtr", &Outer::getConstPtr)
    .addFunction ("getRef", &Outer::getRef)
    .addFunction ("getConstRef", &Outer::getConstRef)
    .addFunction ("getValueConst", &Outer::getValueConst)
    .addFunction ("getPtrConst", &Outer::getPtrConst)
    .addFunction ("getConstPtrConst", &Outer::getConstPtrConst)
    .addFunction ("getRefConst", &Outer::getRefConst)
    .addFunction ("getConstRefConst", &Outer::getConstRefConst)
    .endClass ()
    ;

  Outer outer (Inner (0));
  luabridge::setGlobal (L, &outer, "outer");

  outer.data.data = 0;
  runLua ("outer:getValue ().data = 1");
  ASSERT_EQ (0, outer.data.data);

  outer.data.data = 1;
  runLua ("outer:getPtr ().data = 10");
  ASSERT_EQ (10, outer.data.data);

  outer.data.data = 2;
  ASSERT_THROW (
    runLua ("outer:getConstPtr ().data = 20"),
    std::runtime_error);

  outer.data.data = 3;
  runLua ("outer:getRef().data = 30");
  ASSERT_EQ (30, outer.data.data);

  outer.data.data = 4;
  ASSERT_THROW (
    runLua ("outer:getConstPtr ().data = 40"),
    std::runtime_error);

  outer.data.data = 5;
  runLua ("outer:getValueConst ().data = 50");
  ASSERT_EQ (5, outer.data.data);

  outer.data.data = 6;
  runLua ("outer:getPtrConst ().data = 60");
  ASSERT_EQ (60, outer.data.data);

  outer.data.data = 7;
  ASSERT_THROW (
    runLua ("outer:getConstPtr ().data = 70"),
    std::runtime_error);

  outer.data.data = 8;
  runLua ("outer:getRef().data = 80");
  ASSERT_EQ (80, outer.data.data);

  outer.data.data = 9;
  ASSERT_THROW (
    runLua ("outer:getConstPtr ().data = 90"),
    std::runtime_error);
}
