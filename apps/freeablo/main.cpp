#include <render/render.h>
#include <input/inputmanager.h>
#include <level/level.h>
#include <diabloexe/diabloexe.h>
#include <misc/misc.h>

#include "engine/threadmanager.h"
#include "engine/input.h"
#include "engine/enginemain.h"

#include "falevelgen/levelgen.h"
#include "falevelgen/random.h"

#include "farender/renderer.h"

#include "faworld/world.h"

#include <level/itemmanager.h>

#include "fagui/guimanager.h"


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <fstream>

#include <settings/settings.h>

#include <input/hotkey.h>


namespace bpo = boost::program_options;

bool parseOptions(int argc, char** argv, bpo::variables_map& variables)
{
    boost::program_options::options_description desc("Options");

    desc.add_options()
        ("help,h", "Print help")
        // -1 represents the main menu
        ("level,l", bpo::value<int32_t>()->default_value(-1), "Level number to load (0-16)");

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
            throw bpo::validation_error(
                bpo::validation_error::invalid_option_value, "level");
    }
    catch(bpo::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return false;
    }

    return true;
}


int main(int argc, char** argv)
{
    if (!FAIO::init())
    {
        return EXIT_FAILURE;
    }

    boost::program_options::variables_map variables;

    Engine::EngineMain engine;

    int retval = EXIT_SUCCESS;

    if (parseOptions(argc, argv, variables))
        engine.run(variables);
    else
        retval = EXIT_FAILURE;

    FAIO::quit();
    return retval;
}
