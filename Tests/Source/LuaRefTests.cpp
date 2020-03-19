// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2019, Dmitry Tarakanov
// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
// Copyright 2007, Nathan Reed
// SPDX-License-Identifier: MIT

#include "TestBase.h"

#include <sstream>


struct LuaRefTests : TestBase
{
};

TEST_F (LuaRefTests, ValueAccess)
{
  runLua ("result = true");
  ASSERT_TRUE (result ().isBool ());
  ASSERT_TRUE (result <bool> ());

  runLua ("result = 7");
  ASSERT_TRUE (result ().isNumber ());
  ASSERT_EQ (7u, result <unsigned char> ());
  ASSERT_EQ (7, result <short> ());
  ASSERT_EQ (7u, result <unsigned short> ());
  ASSERT_EQ (7, result <int> ());
  ASSERT_EQ (7u, result <unsigned int> ());
  ASSERT_EQ (7, result <long> ());
  ASSERT_EQ (7u, result <unsigned long> ());
  ASSERT_EQ (7, result <long long> ());
  ASSERT_EQ (7u, result <unsigned long long> ());

  runLua ("result = 3.14");
  ASSERT_TRUE (result ().isNumber ());
  ASSERT_FLOAT_EQ (3.14f, result <float> ());
  ASSERT_DOUBLE_EQ (3.14, result <double> ());

  runLua ("result = 'D'");
  ASSERT_TRUE (result ().isString ());
  ASSERT_EQ ('D', result <char> ());
  ASSERT_EQ ("D", result <std::string> ());
  ASSERT_STREQ ("D", result <const char*> ());

  runLua ("result = 'abc'");
  ASSERT_TRUE (result ().isString ());
  ASSERT_EQ ("abc", result <std::string> ());
  ASSERT_STREQ ("abc", result <char const*> ());

  runLua ("result = function (i) "
          "  result = i + 1 "
          "  return i "
          "end");
  ASSERT_TRUE (result ().isFunction ());
  auto fnResult = result () (41); // Replaces result variable
  ASSERT_TRUE (fnResult.isNumber ());
  ASSERT_EQ (41, fnResult.cast <int> ());
  ASSERT_TRUE (result ().isNumber ());
  ASSERT_EQ (42, result <int> ());
}

TEST_F (LuaRefTests, DictionaryRead)
{
  runLua (
    "result = {"
    "  bool = true,"
    "  int = 5,"
    "  c = 3.14,"
    "  [true] = 'D',"
    "  [8] = 'abc',"
    "  fn = function (i) "
    "    result = i + 1 "
    "    return i "
    "  end"
    "}");

  ASSERT_TRUE (result () ["bool"].isBool ());
  ASSERT_TRUE (result () ["bool"].cast <bool> ());

  ASSERT_TRUE (result () ["int"].isNumber ());
  ASSERT_EQ (5u, result () ["int"].cast <unsigned char> ());
  ASSERT_EQ (5, result () ["int"].cast <short> ());
  ASSERT_EQ (5u, result () ["int"].cast <unsigned short> ());
  ASSERT_EQ (5, result () ["int"].cast <int> ());
  ASSERT_EQ (5u, result () ["int"].cast <unsigned int> ());
  ASSERT_EQ (5, result () ["int"].cast <long> ());
  ASSERT_EQ (5u, result () ["int"].cast <unsigned long> ());
  ASSERT_EQ (5, result () ["int"].cast <long long> ());
  ASSERT_EQ (5u, result () ["int"].cast <unsigned long long> ());

  ASSERT_TRUE (result () ['c'].isNumber ());
  ASSERT_FLOAT_EQ (3.14f, result () ['c'].cast <float> ());
  ASSERT_DOUBLE_EQ (3.14, result () ['c'].cast <double> ());

  ASSERT_TRUE (result () [true].isString ());
  ASSERT_EQ ('D', result () [true].cast <char> ());
  ASSERT_EQ ("D", result () [true].cast <std::string> ());
  ASSERT_STREQ ("D", result () [true].cast <const char*> ());

  ASSERT_TRUE (result () [8].isString ());
  ASSERT_EQ ("abc", result () [8].cast <std::string> ());
  ASSERT_STREQ ("abc", result () [8].cast <char const*> ());

  ASSERT_TRUE (result () ["fn"].isFunction ());
  auto fnResult = result () ["fn"] (41); // Replaces result variable
  ASSERT_TRUE (fnResult.isNumber ());
  ASSERT_EQ (41, fnResult.cast <int> ());
  ASSERT_TRUE (result ().isNumber ());
  ASSERT_EQ (42, result <int> ());
}

TEST_F (LuaRefTests, DictionaryWrite)
{
  runLua ("result = {a = 5}");
  ASSERT_TRUE (result () ["a"].isNumber ());
  ASSERT_EQ (5, result () ["a"].cast <int> ());

  result () ["a"] = 7;
  ASSERT_EQ (7, result () ["a"].cast <int> ());

  runLua ("result = result.a");
  ASSERT_EQ (7, result <int> ());

  runLua ("result = {a = {b = 1}}");
  ASSERT_EQ (1, result () ["a"] ["b"].cast <int> ());

  result () ["a"] ["b"] = 2;
  ASSERT_EQ (2, result () ["a"] ["b"].cast <int> ());
}

struct Class
{
};

TEST_F (LuaRefTests, Comparison)
{
  runLua (
    "function foo () end "
    "local m = {} "
    "m.__eq = function (l, r) return l.a == r.a end "
    "m.__lt = function (l, r) return l.a < r.a end "
    "m.__le = function (l, r) return l.a <= r.a end "
    "t1 = {a = 1} setmetatable (t1, m) "
    "t2 = {a = 2} setmetatable (t2, m) "
    "t3 = {a = 1} setmetatable (t3, m) "
    "t4 = {a = 2} "
  );

  luabridge::getGlobalNamespace (L)
    .beginClass <Class> ("Class")
    .endClass ();

  luabridge::LuaRef nil (L, luabridge::Nil ());
  luabridge::LuaRef boolFalse (L, false);
  luabridge::LuaRef boolTrue (L, true);
  luabridge::LuaRef minus5 (L, -5);
  luabridge::LuaRef numPi (L, 3.14);
  luabridge::LuaRef stringA (L, 'a');
  luabridge::LuaRef stringAB (L, "ab");
  luabridge::LuaRef t1 = luabridge::getGlobal (L, "t1");
  luabridge::LuaRef t2 = luabridge::getGlobal (L, "t2");
  luabridge::LuaRef t3 = luabridge::getGlobal (L, "t3");
  luabridge::LuaRef t4 = luabridge::getGlobal (L, "t4");

  ASSERT_TRUE (nil == nil);

  ASSERT_TRUE (nil < boolFalse);

  ASSERT_TRUE (boolFalse == boolFalse);
  ASSERT_TRUE (boolTrue == boolTrue);

  ASSERT_TRUE (boolTrue < minus5);

  ASSERT_TRUE (minus5 == minus5);
  ASSERT_FALSE (minus5 == numPi);
  ASSERT_TRUE (minus5 < numPi);
  ASSERT_TRUE (minus5 <= numPi);
  ASSERT_FALSE (minus5 > numPi);
  ASSERT_FALSE (minus5 >= numPi);

  ASSERT_TRUE (numPi < stringA);

  ASSERT_TRUE (stringA == stringA);
  ASSERT_FALSE (stringA == stringAB);
  ASSERT_TRUE (stringA < stringAB);
  ASSERT_TRUE (stringA <= stringAB);
  ASSERT_FALSE (stringA > stringAB);
  ASSERT_FALSE (stringA >= stringAB);

  ASSERT_TRUE (stringA < t1);

  ASSERT_TRUE (t1 == t1);
  ASSERT_FALSE (t1 == t2);
  ASSERT_TRUE (t1 == t3);
  ASSERT_FALSE (t1.rawequal (t3));
  ASSERT_FALSE (t1 == t4);
  ASSERT_TRUE (t2 == t2);
  ASSERT_FALSE (t2 == t3);
  ASSERT_FALSE (t2 == t4);
  ASSERT_TRUE (t3 == t3);
  ASSERT_FALSE (t3 == t4);

  ASSERT_FALSE (t1 < t1);
  ASSERT_TRUE (t1 < t2);
  ASSERT_FALSE (t1 < t3);
  ASSERT_FALSE (t2 < t3);

  ASSERT_TRUE (t1 <= t1);
  ASSERT_TRUE (t1 <= t2);
  ASSERT_TRUE (t1 <= t3);
  ASSERT_FALSE (t2 <= t3);

  ASSERT_FALSE (t1 > t1);
  ASSERT_FALSE (t1 > t2);
  ASSERT_FALSE (t1 > t3);
  ASSERT_TRUE (t2 > t3);

  ASSERT_TRUE (t1 >= t1);
  ASSERT_FALSE (t1 >= t2);
  ASSERT_TRUE (t1 >= t3);
  ASSERT_TRUE (t2 >= t3);
}

TEST_F (LuaRefTests, Assignment)
{
  runLua ("value = {a = 5}");
  auto value = luabridge::getGlobal (L, "value");
  ASSERT_TRUE (value.isTable ());
  ASSERT_TRUE (value ["a"].isNumber ());
  ASSERT_EQ (5, value ["a"].cast <int> ());

  value = value ["a"];
  ASSERT_TRUE (value.isNumber ());
  ASSERT_EQ (5, value.cast <int> ());

  value = value;
  ASSERT_EQ (LUA_TNUMBER, value.type ());
  ASSERT_TRUE (value.isNumber ());
  ASSERT_EQ (5, value.cast <int> ());

  runLua ("t = {a = {b = 5}}");
  auto table = luabridge::getGlobal (L, "t");
  luabridge::LuaRef entry = table ["a"];
  luabridge::LuaRef b1 = entry ["b"];
  luabridge::LuaRef b2 = table ["a"] ["b"];
  ASSERT_TRUE (b1 == b2);
}

TEST_F (LuaRefTests, Print)
{
  {
    runLua ("result = true");
    std::ostringstream stream;
    stream << result ();
    ASSERT_EQ ("true", stream.str ());
  }
  {
    runLua ("result = false");
    std::ostringstream stream;
    stream << result ();
    ASSERT_EQ ("false", stream.str ());
  }
  {
    runLua ("result = 5");
    std::ostringstream stream;
    stream << result ();
    ASSERT_EQ ("5", stream.str ());
  }
  {
    runLua ("result = 'abc'");
    std::ostringstream stream;
    stream << result ();
    ASSERT_EQ ("\"abc\"", stream.str ());
  }

  runLua (
    "result = {"
    "  true_ = true,"
    "  false_ = false,"
    "  five = 5,"
    "  abc = 'abc'"
    "}");
  {
    std::ostringstream stream;
    stream << result () ["true_"];
    ASSERT_EQ ("true", stream.str ());
  }
  {
    std::ostringstream stream;
    stream << result () ["false_"];
    ASSERT_EQ ("false", stream.str ());
  }
  {
    std::ostringstream stream;
    stream << result () ["five"];
    ASSERT_EQ ("5", stream.str ());
  }
  {
    std::ostringstream stream;
    stream << result () ["abc"];
    ASSERT_EQ ("\"abc\"", stream.str ());
  }
}
