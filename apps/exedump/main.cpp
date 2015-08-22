#include <diabloexe/diabloexe.h>

#include <iostream>

#include <faio/faio.h>

int main(int, char**)
{
    Settings::Settings settings;
    settings.loadUserSettings();

    std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
    std::string pathMPQ = settings.get<std::string>("Game", "PathMPQ");

    FAIO::init(pathMPQ);

    DiabloExe::DiabloExe exe(pathEXE);
    std::cout << exe.dump();

    FAIO::quit();
    return 0;
}
