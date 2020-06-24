// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2018, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>


int main (int argc, char** argv)
{
  // Disable performance tests by default
  if (argc == 1)
  {
    testing::GTEST_FLAG (filter) = "-PerformanceTests.AllTests";
  }

  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
