//==============================================================================
// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// Copyright 2007, Nathan Reed
// SPDX-License-Identifier: MIT

#include "TestBase.h"

#include "JuceLibraryCode/BinaryData.h"

/**
  Command line version of LuaBridge test suite.
*/

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>

using namespace std;
using namespace luabridge;

//------------------------------------------------------------------------------
/**
  Simple stopwatch for measuring elapsed time.
*/
class Stopwatch
{
private:
  clock_t m_start;

public:
  Stopwatch ()
  {
    start ();
  }

  void start ()
  {
    m_start = clock ();
  }

  double getElapsedSeconds ()
  {
    clock_t now;

    now = clock ();

    return (double (now - m_start)) / CLOCKS_PER_SEC;
  }
};

//------------------------------------------------------------------------------
/**
  Classes used for performance tests.
*/

struct A
{
  A () : data (0), prop (0)
  {
  }

  void mf1 ()
  {
  }

  void mf2 (A*)
  {
  }

  void mf3 (A&)
  {
  }

  virtual void vf1 ()
  {
  }

  int data;

  int prop;
  int getprop () const
  {
    return prop;
  }
  void setprop (int v)
  {
    prop = v;
  }
};

//------------------------------------------------------------------------------

void addToState (lua_State* L)
{
  getGlobalNamespace (L)
    .beginClass <A> ("A")
      .addConstructor <void (*)(void)> ()
      .addFunction ("mf1", &A::mf1)
      .addFunction ("mf2", &A::mf2)
      .addFunction ("mf3", &A::mf3)
      .addFunction ("vf1", &A::vf1)
      .addData ("data",  &A::data)
      .addProperty ("prop", &A::getprop, &A::setprop)
    .endClass ()
    ;
}

void runTests (lua_State* L)
{
  cout.precision (4);

  int result;

  luaL_dostring (L, "a = A()");

  int const trials = 5;

  for (int trial = 0; trial < trials; ++trial)
  {
    result = luaL_loadstring (L, "a:mf1 ()");
    if (result != 0)
      lua_error (L);

    int const N = 10000000;

    Stopwatch sw;

    sw.start ();
    for (int i = 0; i < N; ++i)
    {
      lua_pushvalue (L, -1);
      lua_call (L, 0, 0);
    }

    double const seconds = sw.getElapsedSeconds ();

    cout << "Elapsed time: " << seconds << endl;
  }
}

void runPerformanceTests ()
{
  lua_State* L = luaL_newstate ();
  luaL_openlibs (L);

  addToState (L);
  runTests (L);

  lua_close (L);
}

struct PerformanceTests : TestBase
{
};

TEST_F (PerformanceTests, AllTests)
{
  addToState (L);
  runTests (L);
}
