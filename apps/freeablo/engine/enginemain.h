#ifndef ENGINEMAIN_H
#define ENGINEMAIN_H

#include <boost/program_options.hpp>


namespace Engine
{
    class EngineMain
    {
        public:
        //private:
            /**
             * @brief Handle parsing of command line arguments.
             * @return True if no problems occurred and execution should continue.
             */
            bool parseOptions(int argc, char** argv, boost::program_options::variables_map& variables);
    };
}

#endif // ENGINEMAIN_H
