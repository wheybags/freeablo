// Copyright (c) 2016 Martin Moene
//
// https://github.com/martinmoene/optional-lite
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "optional-main.t.hpp"

#ifndef  optional_HAVE
# define optional_HAVE(FEATURE) ( optional_HAVE_##FEATURE )
#endif

#define optional_PRESENT( x ) \
    std::cout << #x << ": " << x << "\n"

#define optional_ABSENT( x ) \
    std::cout << #x << ": (undefined)\n"

lest::tests & specification()
{
    static lest::tests tests;
    return tests;
}

CASE( "optional-lite version" "[.optional][.version]" )
{
    optional_PRESENT( optional_lite_MAJOR );
    optional_PRESENT( optional_lite_MINOR );
    optional_PRESENT( optional_lite_PATCH );
    optional_PRESENT( optional_lite_VERSION );
}

CASE( "optional-lite configuration" "[.optional][.config]" )
{
    optional_PRESENT( optional_HAVE_STD_OPTIONAL );
    optional_PRESENT( optional_USES_STD_OPTIONAL );
    optional_PRESENT( optional_OPTIONAL_DEFAULT );
    optional_PRESENT( optional_OPTIONAL_NONSTD );
    optional_PRESENT( optional_OPTIONAL_STD );
    optional_PRESENT( optional_CONFIG_SELECT_OPTIONAL );
    optional_PRESENT( optional_CONFIG_NO_EXCEPTIONS );
    optional_PRESENT( optional_CPLUSPLUS );
}

CASE( "__cplusplus" "[.stdc++]" )
{
    optional_PRESENT( __cplusplus );
}

CASE( "compiler version" "[.compiler]" )
{
#if optional_USES_STD_OPTIONAL
    std::cout << "(Compiler version not available: using std::optional)\n";
#else
    optional_PRESENT( optional_COMPILER_CLANG_VERSION );
    optional_PRESENT( optional_COMPILER_GNUC_VERSION );
    optional_PRESENT( optional_COMPILER_MSVC_VERSION );
#endif
}

CASE( "presence of C++ language features" "[.stdlanguage]" )
{
#if optional_USES_STD_OPTIONAL
    std::cout << "(Presence of C++ language features not available: using std::optional)\n";
#else
    optional_PRESENT( optional_HAVE_CONSTEXPR_11 );
    optional_PRESENT( optional_HAVE_NOEXCEPT );
    optional_PRESENT( optional_HAVE_NULLPTR );
    optional_PRESENT( optional_HAVE_REF_QUALIFIER );
    optional_PRESENT( optional_HAVE_CONSTEXPR_14 );
#endif
}

CASE( "presence of C++ library features" "[.stdlibrary]" )
{
#if optional_USES_STD_OPTIONAL
    std::cout << "(Presence of C++ library features not available: using std::optional)\n";
#else
    optional_PRESENT( optional_HAVE_CONDITIONAL );
    optional_PRESENT( optional_HAVE_REMOVE_CV );
    optional_PRESENT( optional_HAVE_TYPE_TRAITS );
    optional_PRESENT( optional_HAVE_TR1_TYPE_TRAITS );
    optional_PRESENT( optional_HAVE_TR1_ADD_POINTER );
#endif
#ifdef _HAS_CPP0X
    optional_PRESENT( _HAS_CPP0X );
#else
    optional_ABSENT(  _HAS_CPP0X );
#endif
}

int main( int argc, char * argv[] )
{
    return lest::run( specification(), argc, argv );
}

#if 0
g++            -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass
g++ -std=c++98 -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass
g++ -std=c++03 -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass
g++ -std=c++0x -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass
g++ -std=c++11 -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass
g++ -std=c++14 -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass
g++ -std=c++17 -I../include -o optional-lite.t.exe optional-lite.t.cpp && optional-lite.t.exe --pass

cl -EHsc -I../include optional-lite.t.cpp && optional-lite.t.exe --pass
#endif

// end of file
