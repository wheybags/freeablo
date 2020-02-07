# optional lite: A single-file header-only version of a C++17-like optional, a nullable object for C++98, C++11 and later

[![Language](https://img.shields.io/badge/C%2B%2B-98/11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![License](https://img.shields.io/badge/license-BSL-blue.svg)](https://opensource.org/licenses/BSL-1.0) [![Build Status](https://travis-ci.org/martinmoene/optional-lite.svg?branch=master)](https://travis-ci.org/martinmoene/optional-lite) [![Build status](https://ci.appveyor.com/api/projects/status/1oq5gjm7bufrv6ib?svg=true)](https://ci.appveyor.com/project/martinmoene/optional-lite) [![Version](https://badge.fury.io/gh/martinmoene%2Foptional-lite.svg)](https://github.com/martinmoene/optional-lite/releases) [![download](https://img.shields.io/badge/latest-download-blue.svg)](https://raw.githubusercontent.com/martinmoene/optional-lite/master/include/nonstd/optional.hpp) [![Conan](https://img.shields.io/badge/on-conan-blue.svg)](https://bintray.com/martinmoene/nonstd-lite/optional-lite%3Anonstd-lite/_latestVersion) [![Try it online](https://img.shields.io/badge/on-wandbox-blue.svg)](https://wandbox.org/permlink/bfZdDT4WerPNZi6b)

**Contents**  
- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Synopsis](#synopsis)
- [Comparison of std::optional, optional lite and Boost.Optional](#comparison-of-stdoptional-optional-lite-and-boostoptional)
- [Reported to work with](#reported-to-work-with)
- [Building the tests](#building-the-tests)
- [Implementation notes](#implementation-notes)
- [Other implementations of optional](#other-implementations-of-optional)
- [Notes and references](#notes-and-references)
- [Appendix](#appendix)


Example usage
-------------

```C++
#include "nonstd/optional.hpp"

#include <cstdlib>
#include <iostream>

using nonstd::optional;
using nonstd::nullopt;

optional<int> to_int( char const * const text )
{
    char * pos = NULL;
    const int value = strtol( text, &pos, 0 );

    return pos == text ? nullopt : optional<int>( value );
}

int main( int argc, char * argv[] )
{
    char * text = argc > 1 ? argv[1] : "42";

    optional<int> oi = to_int( text );

    if ( oi ) std::cout << "'" << text << "' is " << *oi;
    else      std::cout << "'" << text << "' isn't a number";
}
```
### Compile and run
```
prompt>g++ -Wall -Wextra -std=c++03 -I../include -o 01-to_int.exe 01-to_int.cpp && 01-to_int x1
'x1' isn't a number
```

In a nutshell
---------------
**optional lite** is a single-file header-only library to represent optional (nullable) objects and pass them by value. The library aims to provide a [C++17-like optional](http://en.cppreference.com/w/cpp/utility/optional) for use with C++98 and later. If available, std::optional is used. There's also a simpler version, [*optional bare*](https://github.com/martinmoene/optional-bare). Unlike *optional lite*, *optional bare* is limited to default-constructible and copyable types.

**Features and properties of optional lite** are ease of installation (single header), freedom of dependencies other than the standard library and control over object alignment (if needed). *optional lite* shares the approach to in-place tags with [any-lite](https://github.com/martinmoene/any-lite), [expected-lite](https://github.com/martinmoene/expected-lite) and with [variant-lite](https://github.com/martinmoene/variant-lite) and these libraries can be used together.

**Not provided** are reference-type optionals. *optional lite* doesn't handle overloaded *address of* operators.

For more examples, see [this answer on StackOverflow](http://stackoverflow.com/a/16861022) [8] and the [quick start guide](http://www.boost.org/doc/libs/1_57_0/libs/optional/doc/html/boost_optional/quick_start.html) [9] of Boost.Optional (note that its interface differs from *optional lite*).


License
-------
*optional lite* is distributed under the [Boost Software License](LICENSE.txt).


Dependencies
------------
*optional lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation
------------

*optional lite* is a single-file header-only library. Put `optional.hpp` in the [include](include) folder directly into the project source tree or somewhere reachable from your project.

Or, if you use the [conan package manager](https://www.conan.io/), follow these steps:

1. Add *nonstd-lite* to the conan remotes:

        conan remote add nonstd-lite https://api.bintray.com/conan/martinmoene/nonstd-lite

2. Add a reference to *optional-lite* to the *requires* section of your project's `conanfile.txt` file:

        [requires]
        optional-lite/3.1.1@nonstd-lite/testing

3. Run conan's install command:

        conan install


Synopsis
--------

**Contents**  
[Types in namespace nonstd](#types-in-namespace-nonstd)  
[Interface of *optional lite*](#interface-of-optional-lite)  
[Algorithms for *optional lite*](#algorithms-for-optional-lite)  
[Configuration](#configuration)  

### Types and values in namespace nonstd

| Purpose               | Type / value | Object |
|-----------------------|--------------|--------|
| To be, or not         | template< typename T ><br>class **optional**; |&nbsp;|
| Disengaging           | struct **nullopt_t**;                | nullopt_t nullopt; |
| Error reporting       | class **bad_optional_access**;       |&nbsp;  |
| In-place construction | struct **in_place_tag**              | &nbsp; |
| &nbsp;                | **in_place**                         | select type or index for in-place construction |
| &nbsp;                | **in_place_type**                    | select type for in-place construction |
| &emsp;(variant)       | **in_place_index**                   | select index for in-place construction |
| &nbsp;                | **nonstd_lite_in_place_type_t**( T)  | macro for alias template in_place_type_t&lt;T>  |
| &emsp;(variant)       | **nonstd_lite_in_place_index_t**( T )| macro for alias template in_place_index_t&lt;T> |

### Interface of *optional lite*

| Kind         | Std  | Method                                       | Result |
|--------------|------|---------------------------------------------|--------|
| Construction |&nbsp;| **optional**() noexcept                          | default construct a nulled object |
| &nbsp;       |&nbsp;| **optional**( nullopt_t ) noexcept               | explicitly construct a nulled object |
| &nbsp;       |&nbsp;| **optional**( optional const & rhs )             | move-construct from an other optional |
| &nbsp;       | C++11| **optional**( optional && rhs ) noexcept(...)    | move-construct from an other optional |
| &nbsp;       |&nbsp;| **optional**( value_type const & value )         | copy-construct from a value |
| &nbsp;       | C++11| **optional**( value_type && value )              | move-construct from a value |
| &nbsp;       | C++11| **explicit optional**( in_place_type_t&lt;T>, Args&&... args ) | in-place-construct type T |
| &nbsp;       | C++11| **explicit optional**( in_place_type_t&lt;T>, std::initializer_list&lt;U> il, Args&&... args ) | in-place-construct type T |
| Destruction  |&nbsp;| **~optional**()                                  | destruct current content, if any |
| Assignment   |&nbsp;| optional & **operator=**( nullopt_t )            | null the object;<br>destruct current content, if any |
| &nbsp;       |&nbsp;| optional & **operator=**( optional const & rhs ) | copy-assign from other optional;<br>destruct current content, if any |
| &nbsp;       | C++11| optional & **operator=**( optional && rhs )      | move-assign from other optional;<br>destruct current content, if any |
| &nbsp;       | C++11| template< class U, ...><br>**optional & operator=( U && v ) | move-assign from a value;<br>destruct current content, if any |
| &nbsp;       | C++11| template< class... Args ><br>T & **emplace**( Args&&... args ) |  emplace type T |
| &nbsp;       | C++11| template< class U, class... Args ><br>T & **emplace**( std::initializer_list&lt;U> il, Args&&... args ) |  emplace type T |
| Swap         |&nbsp;| void **swap**( optional & rhs ) noexcept(...)    | swap with rhs |
| Content      |&nbsp;| value_type const \* **operator ->**() const       | pointer to current content (const);<br>must contain value |
| &nbsp;       |&nbsp;| value_type \* **operator ->**()                   | pointer to current content (non-const);<br>must contain value |
| &nbsp;       |&nbsp;| value_type const & **operator \***() &            | the current content (const ref);<br>must contain value |
| &nbsp;       |&nbsp;| value_type & **operator \***() &                  | the current content (non-const ref);<br>must contain value |
| &nbsp;       | C++11| value_type const & **operator \***() &&           | the current content (const ref);<br>must contain value |
| &nbsp;       | C++11| value_type & **operator \***() &&                 | the current content (non-const ref);<br>must contain value |
| State        |&nbsp;| operator **bool**() const                        | true if content is present |
| &nbsp;       |&nbsp;| bool **has_value**() const                       | true if content is present |
| &nbsp;       |&nbsp;| value_type const & **value**() &                 | the current content (const ref);<br>throws bad_optional_access if nulled |
| &nbsp;       |&nbsp;| value_type & **value**() &                       | the current content (non-const ref);<br>throws bad_optional_access if nulled |
| &nbsp;       | C++11| value_type const & **value**() &&                | the current content (const ref);<br>throws bad_optional_access if nulled |
| &nbsp;       | C++11| value_type & **value**() &&                      | the current content (non-const ref);<br>throws bad_optional_access if nulled |
| &nbsp;       |<C++11| value_type **value_or**( value_type const & default_value ) const | the value, or default_value if nulled<br>value_type must be copy-constructible |
| &nbsp;       | C++11| value_type **value_or**( value_type && default_value ) &  | the value, or default_value if nulled<br>value_type must be copy-constructible |
| &nbsp;       | C++11| value_type **value_or**( value_type && default_value ) && | the value, or default_value if nulled<br>value_type must be copy-constructible |
| Modifiers    |&nbsp;| void **reset**() noexcept                        | make empty |


### Algorithms for *optional lite*

| Kind                     | Std  | Function |
|--------------------------|------|----------|
| Relational operators     |&nbsp;| &nbsp;   |
| ==                       |&nbsp;| template< typename T ><br>bool **operator==**( optional<T> const & x, optional<T> const & y ) |
| !=                       |&nbsp;| template< typename T ><br>bool **operator!=**( optional<T> const & x, optional<T> const & y ) |
| <                        |&nbsp;| template< typename T ><br>bool **operator<**( optional<T> const & x, optional<T> const & y )  |
| >                        |&nbsp;| template< typename T ><br>bool **operator>**( optional<T> const & x, optional<T> const & y )  |
| <=                       |&nbsp;| template< typename T ><br>bool **operator<=*( optional<T> const & x, optional<T> const & y ) |
| >=                       |&nbsp;| template< typename T ><br>bool **operator>=*( optional<T> const & x, optional<T> const & y ) |
| Comparison with nullopt  |&nbsp;| &nbsp;   |
| ==                       |&nbsp;| template< typename T ><br>bool **operator==**( optional<T> const & x, nullopt_t ) noexcept |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator==**( nullopt_t, optional<T> const & x ) noexcept |
| !=                       |&nbsp;| template< typename T ><br>bool **operator!=**( optional<T> const & x, nullopt_t ) noexcept |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator!=**( nullopt_t, optional<T> const & x ) noexcept |
| <                        |&nbsp;| template< typename T ><br>bool **operator<**( optional<T> const &, nullopt_t ) noexcept    |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator<**( nullopt_t, optional<T> const & x ) noexcept  |
| <=                       |&nbsp;| template< typename T ><br>bool **operator<=**( optional<T> const & x, nullopt_t ) noexcept |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator<=**( nullopt_t, optional<T> const & ) noexcept   |
| >                        |&nbsp;| template< typename T ><br>bool **operator>**( optional<T> const & x, nullopt_t ) noexcept  |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator>**( nullopt_t, optional<T> const & ) noexcept    |
| >=                       |&nbsp;| template< typename T ><br>bool **operator>=**( optional<T> const &, nullopt_t ) noexcept   |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator>=**( nullopt_t, optional<T> const & x ) noexcept |
| Comparison with T        |&nbsp;| &nbsp;   |
| ==                       |&nbsp;| template< typename T ><br>bool **operator==**( optional<T> const & x, const T& v )  |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator==**( T const & v, optional<T> const & x ) |
| !=                       |&nbsp;| template< typename T ><br>bool **operator!=**( optional<T> const & x, const T& v )  |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator!=**( T const & v, optional<T> const & x ) |
| <                        |&nbsp;| template< typename T ><br>bool **operator<**( optional<T> const & x, const T& v )   |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator<**( T const & v, optional<T> const & x )  |
| <=                       |&nbsp;| template< typename T ><br>bool **operator<=**( optional<T> const & x, const T& v )  |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator<=**( T const & v, optional<T> const & x ) |
| >                        |&nbsp;| template< typename T ><br>bool **operator>**( optional<T> const & x, const T& v )   |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator>**( T const & v, optional<T> const & x )  |
| >=                       |&nbsp;| template< typename T ><br>bool **operator>=**( optional<T> const & x, const T& v )  |
| &nbsp;                   |&nbsp;| template< typename T ><br>bool **operator>=**( T const & v, optional<T> const & x ) |
| Specialized algorithms   |&nbsp;| &nbsp;   |
| swap                     |&nbsp;| template< typename T ><br>void **swap**( optional<T> & x, optional<T> & y ) noexcept(...) |
| create                   |<C++11| template< typename T ><br>optional&lt;T> **make_optional**( T const & v )      |
| &nbsp;                   | C++11| template< class T ><br>optional< typename std::decay&lt;T>::type > **make_optional**( T && v ) |
| &nbsp;                   | C++11| template< class T, class...Args ><br>optional&lt;T> **make_optional**( Args&&... args ) |
| &nbsp;                   | C++11| template< class T, class U, class... Args ><br>optional&lt;T> **make_optional**( std::initializer_list&lt;U> il, Args&&... args ) |
| hash                     | C++11| template< class T ><br>class **hash**< nonstd::optional&lt;T> > |


### Configuration

#### Standard selection macro
\-D<b>optional\_CPLUSPLUS</b>=199711L  
Define this macro to override the auto-detection of the supported C++ standard, if your compiler does not set the `__cplusplus` macro correctly.

#### Select `std::optional` or `nonstd::optional`
At default, *optional lite* uses `std::optional` if it is available and lets you use it via namespace `nonstd`. You can however override this default and explicitly request to use `std::optional` or optional lite's `nonstd::optional` as `nonstd::optional` via the following macros.

-D<b>optional\_CONFIG\_SELECT\_OPTIONAL</b>=optional_OPTIONAL_DEFAULT  
Define this to `optional_OPTIONAL_STD` to select `std::optional` as `nonstd::optional`. Define this to `optional_OPTIONAL_NONSTD` to select `nonstd::optional` as `nonstd::optional`. Default is undefined, which has the same effect as defining to `optional_OPTIONAL_DEFAULT`.

#### Disable exceptions
-D<b>optional_CONFIG_NO_EXCEPTIONS</b>=0
Define this to 1 if you want to compile without exceptions. If not defined, the header tries and detect if exceptions have been disabled (e.g. via `-fno-exceptions`). Default is undefined.

#### Macros to control alignment

If *optional lite* is compiled as C++11 or later, C++11 alignment facilities are used for storage of the underlying object. When compiled as pre-C++11, *optional lite* tries to determine proper alignment itself. If this doesn't work out, you can control alignment via the following macros. See also section [Implementation notes](#implementation-notes).

-D<b>optional_CONFIG_MAX_ALIGN_HACK</b>=0  
Define this to 1 to use the *max align hack* for alignment. Default is 0.

-D<b>optional_CONFIG_ALIGN_AS</b>=*pod-type*  
Define this to the *pod-type* you want to align to (no default).

-D<b>optional_CONFIG_ALIGN_AS_FALLBACK</b>=*pod-type*  
Define this to the *pod-type* to use for alignment if the algorithm of *optional lite* cannot find a suitable POD type to use for alignment. Default is double.


Comparison of std::optional, optional lite and Boost.Optional
-------------------------------------------------------------

*optional lite* is inspired on std::optional, which in turn is inspired on Boost.Optional. Here are the significant differences.

| Aspect                            | std::optional         | optional lite        | Boost.Optional |
|-----------------------------------|-----------------------|----------------------|----------------|
| Move semantics                    | yes                   | C++11                | no             |
| noexcept                          | yes                   | C++11                | no             |
| Hash support                      | yes                   | C++11                | no             |
| Throwing value accessor           | yes                   | yes                  | no             |
| Literal type	                    | partially             | C++11/14             | no             |
| In-place construction	            | emplace, tag in_place | emplace, tag in_place| utility in_place_factory |
| Disengaged state tag	            | nullopt	            | nullopt              | none           |
| optional references               | no                    | no                   | yes            |
| Conversion from optional&lt;U\><br>to optional&lt;T\>    | no | no               | yes            |
| Duplicated interface functions 1) | no                    | no                   | yes            |
| Explicit convert to ptr (get_ptr)	| no                    | no                   | yes            |

1) is_initialized(), reset(), get().


Reported to work with
---------------------
The table below mentions the compiler versions *optional lite* is reported to work with.

OS        | Compiler   | Versions |
---------:|:-----------|:---------|
Windows   | Clang/LLVM | ?        |
&nbsp;    | GCC        | 5.2.0    |
&nbsp;    | Visual C++<br>(Visual Studio)| 8 (2005), 10 (2010), 11 (2012),<br>12 (2013), 14 (2015), 14 (2017) |
GNU/Linux | Clang/LLVM | 3.5.0, 3.6.0, 7.0.0 |
&nbsp;    | GCC        | 4.8.4, 5, 6, 8 |
&nbsp;    | ICC        | 19       |
OS X      | ?          | ?        |


Building the tests
------------------
To build the tests you need:

- [CMake](http://cmake.org), version 2.8.12 or later to be installed and in your PATH.
- A [suitable compiler](#reported-to-work-with).

The [*lest* test framework](https://github.com/martinmoene/lest)  is included in the [test folder](test).

The following steps assume that the [*optional lite* source code](https://github.com/martinmoene/optional-lite) has been cloned into a directory named `c:\optional-lite`.

1. Create a directory for the build outputs for a particular architecture.
Here we use c:\optional-lite\build-win-x86-vc10.

        cd c:\optional-lite
        md build-win-x86-vc10
        cd build-win-x86-vc10

2. Configure CMake to use the compiler of your choice (run `cmake --help` for a list).

        cmake -G "Visual Studio 10 2010" -DOPTIONAL_LITE_OPT_BUILD_TESTS=ON ..

3. Build the test suite in the Debug configuration (alternatively use Release).    

        cmake --build . --config Debug

4. Run the test suite.    

        ctest -V -C Debug

All tests should pass, indicating your platform is supported and you are ready to use *optional lite*.


Implementation notes
--------------------

### Object allocation and alignment

*optional lite* reserves POD-type storage for an object of the underlying type inside a union to prevent unwanted construction and uses placement new to construct the object when required. Using non-placement new (malloc) to  obtain storage, ensures that the memory is properly aligned for the object's type, whereas that's not the case with placement new.

If you access data that's not properly aligned, it 1) may take longer than when it is properly aligned (on x86 processors), or 2) it may terminate the program immediately (many other processors).

Although the C++ standard does not guarantee that all user-defined types have the alignment of some POD type, in practice it's likely they do [8, part 2].

If *optional lite* is compiled as C++11 or later, C++11 alignment facilities are used for storage of the underlying object. When compiling as pre-C++11, *optional lite* tries to determine proper alignment using meta programming. If this doesn't work out, you can control alignment via three macros.

*optional lite* uses the following rules for alignment:

1. If the program compiles as C++11 or later, C++11 alignment facilities  are used.

2. If you define -D<b>optional_CONFIG_MAX_ALIGN_HACK</b>=1 the underlying type is aligned as the most restricted type in `struct max_align_t`. This potentially wastes many bytes per optional if the actually required alignment is much less, e.g. 24 bytes used instead of the 2 bytes required.

3. If you define -D<b>optional_CONFIG_ALIGN_AS</b>=*pod-type* the underlying type is aligned as *pod-type*. It's your obligation to specify a type with proper alignment.

4. If you define -D<b>optional_CONFIG_ALIGN_AS_FALLBACK</b>=*pod-type* the fallback type for alignment of rule 5 below becomes *pod-type*. It's your obligation to specify a type with proper alignment.

5. At default, *optional lite* tries to find a POD type with the same alignment as the underlying type.

	The algorithm for alignment of 5. is:
	- Determine the alignment A of the underlying type using `alignment_of<>`.
	- Find a POD type from the list `alignment_types` with exactly alignment A.
	- If no such POD type is found, use a type with a relatively strict alignment requirement such as double; this type is specified in  `optional_CONFIG_ALIGN_AS_FALLBACK` (default double).

Note that the algorithm of 5. differs from the one Andrei Alexandrescu uses in [8, part 2].

The class template `alignment_of<>` is gleaned from [Boost.TypeTraits, alignment_of](http://www.boost.org/doc/libs/1_57_0/libs/type_traits/doc/html/boost_typetraits/reference/alignment_of.html) [11]. The storage type `storage_t<>` is adapted from the one I created for [spike-expected, expected lite](https://github.com/martinmoene/spike-expected) [13].

For more information on constructed unions and alignment, see [8-12].


Other implementations of optional
---------------------------------
- Isabella Muerte. [MNMLSTC Core](https://github.com/mnmlstc/core) (C++11).
- Andrzej Krzemieński. [optional (nullable) objects for C++14](https://github.com/akrzemi1/Optional). Reference implementation.
- Simon Brand. [C++11/14/17 std::optional with functional-style extensions](https://github.com/TartanLlama/optional).

Notes and references
--------------------
[1] CppReference. [Optional](http://en.cppreference.com/w/cpp/utility/optional).

[2] ISO/IEC WG21. [N4606, section 20.6 Optional objects](http://wg21.link/n4606). July 2016.

[3] Fernando Cacciola, Andrzej Krzemieński. [A proposal to add a utility class to represent optional objects (Revision 5)](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3793.html).

[4] Andrzej Krzemieński. [optional (nullable) objects for C++14](https://github.com/akrzemi1/Optional). Reference implementation on GitHub.

[5] Simon Brand. [P0798R0: Monadic operations for std::optional](https://wg21.tartanllama.xyz/monadic-optional).

[6] Simon Brand. [C++11/14/17 std::optional with functional-style extensions ](https://github.com/TartanLlama/optional). Reference implementation on GitHub.

[7] Fernando Cacciola. [Boost.Optional library](http://www.boost.org/doc/libs/1_49_0/libs/optional/doc/html/index.html).

[8] StackOverflow. [How should one use std::optional?](http://stackoverflow.com/a/16861022). Answer by Timothy Shields. 31 May 2013.

[9] Fernando Cacciola. [Boost.Optional Quick start guide](http://www.boost.org/doc/libs/1_57_0/libs/optional/doc/html/boost_optional/quick_start.html).

[10] Andrei Alexandrescu. [Generic<Programming>: Discriminated Unions part 1](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2004/alexandr/alexandr.htm), [part 2](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2006/alexandr/alexandr.htm), [part 3](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2008/alexandr/alexandr.htm). April 2002.

[11] Herb Sutter. [Style Case Study #3: Construction Unions](http://www.gotw.ca/gotw/085.htm). GotW #85. 2009

[12] Kevin T. Manley. [Using Constructed Types in C++ Unions](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/0208/manley/manley.htm). C/C++ Users Journal, 20(8), August 2002.

[13] StackOverflow. [Determining maximum possible alignment in C++](http://stackoverflow.com/a/3126992).

[14] [Boost.TypeTraits, alignment_of](http://www.boost.org/doc/libs/1_57_0/libs/type_traits/doc/html/boost_typetraits/reference/alignment_of.html) ( [code](http://www.boost.org/doc/libs/1_57_0/boost/type_traits/alignment_of.hpp) ).

[15] Martin Moene. [spike-expected](https://github.com/martinmoene/spike-expected) ([expected-lite.hpp](https://github.com/martinmoene/spike-expected/blob/master/exception_ptr_lite.hpp)).


Appendix
--------

### A.1 Compile-time information

The version of *optional lite* is available via tag `[.version]`. The following tags are available for information on the compiler and on the C++ standard library used: `[.compiler]`, `[.stdc++]`, `[.stdlanguage]` and `[.stdlibrary]`.

### A.2 Optional Lite test specification

```
union: A C++03 union can only contain POD types
optional: Allows to default construct an empty optional (1a)
optional: Allows to explicitly construct a disengaged, empty optional via nullopt (1b)
optional: Allows to default construct an empty optional with a non-default-constructible (1a)
optional: Allows to copy-construct from empty optional (2)
optional: Allows to move-construct from empty optional (C++11, 3)
optional: Allows to copy-construct from empty optional, explicit converting (C++11, 4a)
optional: Allows to copy-construct from empty optional, non-explicit converting (4b)
optional: Allows to move-construct from empty optional, explicit converting (C++11, 5a)
optional: Allows to move-construct from empty optional, non-explicit converting (C++11, 5a)
optional: Allows to copy-construct from non-empty optional (2)
optional: Allows to copy-construct from non-empty optional, explicit converting (C++11, 4a)
optional: Allows to copy-construct from non-empty optional, non-explicit converting (4b)
optional: Allows to move-construct from non-empty optional (C++11, 3)
optional: Allows to move-construct from non-empty optional, explicit converting (C++11, 5a)
optional: Allows to move-construct from non-empty optional, non-explicit converting (C++11, 5b)
optional: Allows to copy-construct from literal value (8)
optional: Allows to copy-construct from literal value, converting (8)
optional: Allows to copy-construct from value (8)
optional: Allows to copy-construct from value, converting (8)
optional: Allows to move-construct from value (C++11, 8b)
optional: Allows to move-construct from value, explicit converting (C++11, 8a)
optional: Allows to move-construct from value, non-explicit converting (C++11, 8b)
optional: Allows to in-place construct from literal value (C++11, 6)
optional: Allows to in-place copy-construct from value (C++11, 6)
optional: Allows to in-place move-construct from value (C++11, 6)
optional: Allows to in-place copy-construct from initializer-list (C++11, 7)
optional: Allows to in-place move-construct from initializer-list (C++11, 7)
optional: Allows to assign nullopt to disengage (1)
optional: Allows to copy-assign from/to engaged and disengaged optionals (2)
optional: Allows to move-assign from/to engaged and disengaged optionals (C++11, 3)
optional: Allows to copy-assign from/to engaged and disengaged optionals, converting, (5)
optional: Allows to move-assign from/to engaged and disengaged optionals, converting (C++11, 6)
optional: Allows to copy-assign from literal value (4)
optional: Allows to copy-assign from value (4)
optional: Allows to move-assign from value (C++11, 4)
optional: Allows to copy-emplace content from arguments (C++11, 7)
optional: Allows to move-emplace content from arguments (C++11, 7)
optional: Allows to copy-emplace content from intializer-list and arguments (C++11, 8)
optional: Allows to move-emplace content from intializer-list and arguments (C++11, 8)
optional: Allows to swap with other optional (member)
optional: Allows to obtain value via operator->()
optional: Allows to obtain moved-value via operator->() (C++11)
optional: Allows to obtain value via operator*()
optional: Allows to obtain moved-value via operator*() (C++11)
optional: Allows to obtain has_value() via operator bool()
optional: Allows to obtain value via value()
optional: Allows to obtain moved-value via value() (C++11)
optional: Allows to obtain value or default via value_or()
optional: Allows to obtain moved-value or moved-default via value_or() (C++11)
optional: Throws bad_optional_access at disengaged access
optional: Throws bad_optional_access with non-empty what()
optional: Allows to reset content
optional: Allows to swaps engage state and values (non-member)
optional: Provides relational operators
optional: Provides mixed-type relational operators
make_optional: Allows to copy-construct optional
make_optional: Allows to move-construct optional (C++11)
make_optional: Allows to in-place copy-construct optional from arguments (C++11)
make_optional: Allows to in-place move-construct optional from arguments (C++11)
make_optional: Allows to in-place copy-construct optional from initializer-list and arguments (C++11)
make_optional: Allows to in-place move-construct optional from initializer-list and arguments (C++11)
std::hash<>: Allows to obtain hash (C++11)
```
