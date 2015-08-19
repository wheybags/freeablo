#include "enginemain.h"

#include <iostream>


namespace bpo = boost::program_options;


namespace Engine
{
    /**
     * @brief Handle parsing of command line arguments.
     * @return True if no problems occurred and execution should continue.
     */
    bool EngineMain::parseOptions(int argc, char** argv, bpo::variables_map& variables)
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
}

