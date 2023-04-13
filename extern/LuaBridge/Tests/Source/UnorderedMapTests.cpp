// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2019, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#include "TestBase.h"

#include "LuaBridge/UnorderedMap.h"

#include <unordered_map>


struct UnorderedMapTests : TestBase
{
};

namespace {

struct Data
{
  /* explicit */ Data(int i) : i(i) {}

  int i;
};

} // namespace

namespace std {

template <>
struct hash <Data>
{
  size_t operator() (const Data& value) const noexcept
  {
    return 0; // Don't care about hash collisions
  }
};

template <>
struct hash <::luabridge::LuaRef>
{
  size_t operator() (const ::luabridge::LuaRef& value) const
  {
    return 0; // Don't care about hash collisions
  }
};

} // namespace std

TEST_F (UnorderedMapTests, LuaRef)
{
  {
    runLua ("result = {[false] = true, a = 'abc', [1] = 5, [3.14] = -1.1}");

    using Map = std::unordered_map <luabridge::LuaRef, luabridge::LuaRef>;
    Map expected {
      {luabridge::LuaRef (L, false), luabridge::LuaRef (L, true)},
      {luabridge::LuaRef (L, 'a'), luabridge::LuaRef (L, "abc")},
      {luabridge::LuaRef (L, 1), luabridge::LuaRef (L, 5)},
      {luabridge::LuaRef (L, 3.14), luabridge::LuaRef (L, -1.1)},
    };
    Map actual = result ();
    ASSERT_EQ (expected, actual);
    ASSERT_EQ (expected, result <Map> ());
  }

  {
    runLua ("result = {'a', 'b', 'c'}");

    using Int2Char = std::unordered_map <int, char>;
    Int2Char expected {{1, 'a'}, {2, 'b'}, {3, 'c'}};
    Int2Char actual = result ();
    ASSERT_EQ (expected, actual);
    ASSERT_EQ (expected, result <Int2Char> ());
  }
}

TEST_F (UnorderedMapTests, PassToFunction)
{
  runLua (
    "function foo (map) "
    "  result = map "
    "end");

  auto foo = luabridge::getGlobal (L, "foo");
  using Int2Bool = std::unordered_map <int, bool>;

  resetResult ();

  Int2Bool lvalue {{10, false}, {20, true}, {30, true}};
  foo (lvalue);
  ASSERT_TRUE (result ().isTable ());
  ASSERT_EQ (lvalue, result <Int2Bool> ());

  resetResult ();

  const Int2Bool constLvalue = lvalue;
  foo (constLvalue);
  ASSERT_TRUE (result ().isTable ());
  ASSERT_EQ (constLvalue, result <Int2Bool> ());
}

namespace {

bool operator== (const Data& lhs, const Data& rhs)
{
  return lhs.i == rhs.i;
}

bool operator< (const Data& lhs, const Data& rhs)
{
  return lhs.i < rhs.i;
}

std::ostream& operator<< (std::ostream& lhs, const Data& rhs)
{
  lhs << "{" << rhs.i << "}";
  return lhs;
}

std::unordered_map <Data, Data> processValues (const std::unordered_map <Data, Data>& data)
{
  return data;
}

std::unordered_map <Data, Data> processPointers (const std::unordered_map <Data, const Data*>& data)
{
  std::unordered_map <Data, Data> result;
  for (const auto& item : data)
  {
    result.emplace (item.first, *item.second);
  }
  return result;
}

} // namespace

TEST_F (UnorderedMapTests, PassFromLua)
{
  luabridge::getGlobalNamespace (L)
    .beginClass <Data> ("Data")
    .addConstructor <void (*) (int)> ()
    .endClass ()
    .addFunction ("processValues", &processValues)
    .addFunction ("processPointers", &processPointers);

  {
    resetResult ();
    runLua ("result = processValues ({[Data (-1)] = Data (2)})");
    std::unordered_map <Data, Data> expected {{Data (-1), Data (2)}};
    const auto actual = result <std::unordered_map <Data, Data>> ();
    ASSERT_EQ (expected, actual);
  }

  {
    resetResult ();
    runLua ("result = processValues ({[Data (3)] = Data (-4)})");
    std::unordered_map <Data, Data> expected {{Data (3), Data (-4)}};
    const auto actual = result <std::unordered_map <Data, Data>> ();
    ASSERT_EQ (expected, actual);
  }
}
