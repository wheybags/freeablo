#include <diabloexe/diabloexe.h>

#include <iostream>

int main(int, char**)
{
    DiabloExe::DiabloExe exe;
    std::cout << exe.dump();

    return 0;
}
