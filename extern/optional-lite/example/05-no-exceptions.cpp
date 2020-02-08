#include "nonstd/optional.hpp"

using nonstd::optional;

int main()
{
    optional<int> v;

    int x = v.value();  // asserts (normally throws)
}

// cl -nologo -I../include 05-no-exceptions.cpp && 05-no-exceptions
// g++ -Wall -fno-exceptions -I../include -o 05-no-exceptions 05-no-exceptions.cpp && 05-no-exceptions
