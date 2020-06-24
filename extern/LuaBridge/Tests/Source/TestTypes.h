// https://github.com/vinniefalco/LuaBridge
//
// Copyright 2019, Dmitry Tarakanov
// SPDX-License-Identifier: MIT

#pragma once

#include <gtest/gtest.h>

#include <vector>
#include <string>


using TestTypes = ::testing::Types <
  bool,
  char,
  unsigned char,
  short,
  unsigned short,
  int,
  unsigned int,
  long,
  unsigned long,
  long long,
  unsigned long long,
  float,
  double
>;

template <class T>
struct TypeTraits;

template <>
struct TypeTraits <bool>
{
  static std::vector <bool> values () {return {true, false, true};}
  static std::string list () {return "true, false, true";}
};

template <>
struct TypeTraits <char>
{
  static std::vector <char> values () {return {'a', 'b', 'c'};}
  static std::string list () {return "'a', 'b', 'c'";}
};

template <>
struct TypeTraits <unsigned char>
{
  static std::vector <unsigned char> values () {return {1, 2, 3};}
  static std::string list () {return "1, 2, 3";}
};

template <>
struct TypeTraits <short>
{
  static std::vector <short> values () {return {1, -2, 3};}
  static std::string list () {return "1, -2, 3";}
};

template <>
struct TypeTraits <unsigned short>
{
  static std::vector <unsigned short> values () {return {1, 2, 3};}
  static std::string list () {return "1, 2, 3";}
};

template <>
struct TypeTraits <int>
{
  static std::vector <int> values () {return {1, -2, 3};}
  static std::string list () {return "1, -2, 3";}
};

template <>
struct TypeTraits <unsigned int>
{
  static std::vector <unsigned int> values () {return {1, 2, 3};}
  static std::string list () {return "1, 2, 3";}
};

template <>
struct TypeTraits <long>
{
  static std::vector <long> values () {return {1, -2, 3};}
  static std::string list () {return "1, -2, 3";}
};

template <>
struct TypeTraits <unsigned long>
{
  static std::vector <unsigned long> values () {return {1, 2, 3};}
  static std::string list () {return "1, 2, 3";}
};

template <>
struct TypeTraits <long long>
{
  static std::vector <long long> values () {return {1, -2, 3};}
  static std::string list () {return "1, -2, 3";}
};

template <>
struct TypeTraits <unsigned long long>
{
  static std::vector <unsigned long long> values () {return {1, 2, 3};}
  static std::string list () {return "1, 2, 3";}
};

template <>
struct TypeTraits <float>
{
  static std::vector <float> values () {return {1.2f, -2.5f, 3.14f};}
  static std::string list () {return "1.2, -2.5, 3.14";}
};

template <>
struct TypeTraits <double>
{
  static std::vector <double> values () {return {1.2, -2.5, 3.14};}
  static std::string list () {return "1.2, -2.5, 3.14";}
};
