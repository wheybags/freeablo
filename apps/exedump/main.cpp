#include <diabloexe/diabloexe.h>

#include <iostream>

#include <faio/faio.h>

int main(int, char**)
{
    FAIO::init();

    DiabloExe::DiabloExe exe;
    std::cout << exe.dump();

    FAIO::quit();
    return 0;
}
