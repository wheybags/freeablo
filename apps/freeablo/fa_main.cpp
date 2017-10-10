#include <iostream>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include <settings/settings.h>
#include <faio/fafileobject.h>

#include "engine/enginemain.h"

namespace bpo = boost::program_options;
bool parseOptions(int argc, char** argv, bpo::variables_map& variables)
{
    boost::program_options::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help")
        // -1 represents the main menu
        ("level,l", bpo::value<int32_t>()->default_value(-1), "Level number to load (0-16)")
        ("character,c", bpo::value<std::string>()->default_value("Warrior"), "Choose Warrior, Rogue or Sorcerer")
        ("mode,m", bpo::value<std::string>()->default_value("server"), "server or client")
        ("invuln", bpo::value<std::string>()->default_value("off"), "on or off");

    try
    {
        bpo::store(bpo::parse_command_line(argc, argv, desc), variables);

        if(variables.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }

        bpo::notify(variables);

        const int32_t dLvl = variables["level"].as<int32_t>();

        if(dLvl > 16)
            throw bpo::error("There is no level after 16");
    }
    catch(bpo::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return false;
    }

    return true;
}


int fa_main(int argc, char** argv)
{
    Settings::Settings settings;
    if(!settings.loadUserSettings())
        return EXIT_FAILURE;

    if (!FAIO::init(settings.get<std::string>("Game","PathMPQ")))
    {
        return EXIT_FAILURE;
    }

    Engine::EngineMain engine;

    int retval = EXIT_SUCCESS;

    boost::program_options::variables_map variables;
    if (parseOptions(argc, argv, variables))
        engine.run(variables);
    else
        retval = EXIT_FAILURE;

    FAIO::FAFileObject::quit();
    return retval;
}
