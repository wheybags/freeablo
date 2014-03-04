#include <diabloexe/diabloexe.h>

#include <iostream>

int main(int argc, char** argv)
{
    DiabloExe::DiabloExe exe;
    std::cout << exe.dump();

    return 0;
}
