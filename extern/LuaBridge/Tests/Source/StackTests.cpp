// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2020, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#include "TestBase.h"

struct StackTests : TestBase
{
};

TEST_F (StackTests, IntegralTypes)
{
  luabridge::push (L, true);
  ASSERT_TRUE (luabridge::isInstance <bool> (L, -1));
  ASSERT_FALSE (luabridge::isInstance <int> (L, -1));

  luabridge::push (L, 5);
  ASSERT_TRUE (luabridge::isInstance <int> (L, -1));
  ASSERT_FALSE (luabridge::isInstance <bool> (L, -1));

  ASSERT_TRUE (luabridge::isInstance <bool>(L, -2));
}
