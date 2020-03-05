#include "settings/settings.h"
#include <diabloexe/diabloexe.h>
#include <faio/fafileobject.h>
#include <iostream>
#include <misc/misc.h>

int main(int, char** argv)
{
    Misc::saveArgv0(argv[0]);

    Settings::Settings settings;
    settings.loadUserSettings();

    std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
    std::string pathMPQ = settings.get<std::string>("Game", "PathMPQ");

    FAIO::init(pathMPQ);

    DiabloExe::DiabloExe exe(pathEXE);
    std::cout << exe.dump();

    FAIO::FAFileObject::quit();
    return 0;
}
