// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2018, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#include "TestBase.h"

#include "LuaBridge/detail/Iterator.h"

struct IteratorTests : TestBase
{
};

TEST_F (IteratorTests, DictionaryIteration)
{
  runLua (
    "result = {"
    "  bool = true,"
    "  int = 5,"
    "  c = 3.14,"
    "  [true] = 'D',"
    "  [8] = 'abc',"
    "  fn = function (i)"
    "    result = i + 1"
    "  end"
    "}");

  std::map <luabridge::LuaRef, luabridge::LuaRef> expected {
    {{L, "bool"}, {L, true}},
    {{L, "int"}, {L, 5}},
    {{L, 'c'}, {L, 3.14}},
    {{L, true}, {L, 'D'}},
    {{L, 8}, {L, "abc"}},
    {{L, "fn"}, {L, result () ["fn"]}},
  };

  std::map <luabridge::LuaRef, luabridge::LuaRef> actual;

  for (luabridge::Iterator iterator (result ()); !iterator.isNil (); ++iterator)
  {
    actual.emplace(iterator.key (), iterator.value ());
  }

  ASSERT_EQ (expected, actual);

  actual.clear ();

  for (auto&& pair : pairs (result ()))
  {
    actual.emplace (pair.first, pair.second);
  }

  ASSERT_EQ (expected, actual);
}

TEST_F (IteratorTests, SequenceIteration)
{
  runLua (
    "result = {"
    "  true,"
    "  5,"
    "  3.14,"
    "  'D',"
    "  'abc',"
    "  function (i)"
    "    result = i + 1"
    "  end"
    "}");

  std::map <luabridge::LuaRef, luabridge::LuaRef> expected {
    {{L, 1}, {L, true}},
    {{L, 2}, {L, 5}},
    {{L, 3}, {L, 3.14}},
    {{L, 4}, {L, 'D'}},
    {{L, 5}, {L, "abc"}},
    {{L, 6}, {L, result () [6]}},
  };

  std::map <luabridge::LuaRef, luabridge::LuaRef> actual;

  for (luabridge::Iterator iterator (result ()); !iterator.isNil (); ++iterator)
  {
    actual.emplace (iterator.key (), iterator.value ());
  }

  ASSERT_EQ (expected, actual);

  actual.clear ();

  for (auto&& pair : pairs (result ()))
  {
    actual.emplace (pair.first, pair.second);
  }

  ASSERT_EQ (expected, actual);
}
