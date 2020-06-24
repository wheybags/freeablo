#include "dumptileset.h"
#include "settings/settings.h"
#include <cel/celdecoder.h>
#include <cxxopts.hpp>
#include <diabloexe/diabloexe.h>
#include <faio/fafileobject.h>
#include <fmt/format.h>
#include <iostream>
#include <misc/misc.h>

int main(int argc, char** argv)
{
    Misc::saveArgv0(argv[0]);

    Settings::Settings settings;
    settings.loadUserSettings();

    std::string pathMPQ = settings.get<std::string>("Game", "PathMPQ");

    FAIO::ScopedInitFAIO faioInit(pathMPQ);

    cxxopts::Options desc("Options");
    desc.add_options()("h,help", "Print help")("dump-data", "Dump game data from exe as text to stdout")(
        "dump-tiles", "Path to dump min and til tiles as pngs", cxxopts::value<std::string>());

    cxxopts::ParseResult variables;
    try
    {
        variables = desc.parse(argc, argv);
    }
    catch (cxxopts::OptionParseException& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc.help() << std::endl;
        return 1;
    }

    if (variables.arguments().empty() || variables.count("help"))
    {
        std::cout << desc.help() << std::endl;
        return 0;
    }

    if (variables.count("dump-data"))
    {
        std::string pathEXE = settings.get<std::string>("Game", "PathEXE");
        DiabloExe::DiabloExe exe(pathEXE);
        std::cout << exe.dump();
    }

    if (variables.count("dump-tiles"))
    {
        Cel::CelDecoder::loadConfigFiles();
        dumpTiles(variables["dump-tiles"].as<std::string>());
    }

    return 0;
}
