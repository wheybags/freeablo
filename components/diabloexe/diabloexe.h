#ifndef DIABLO_EXE_H
#define DIABLO_EXE_H

#include <string>
#include <map>

#include <boost/property_tree/ptree.hpp>

#include <faio/faio.h>

#include "monster.h"

namespace DiabloExe
{
    class DiabloExe
    {
        public:
            DiabloExe();

            std::map<std::string, Monster> monsters;

            std::string dump();

        private:
            std::string getMD5();
            std::string getVersion();

            void loadMonsters(FAIO::FAFile* exe, boost::property_tree::ptree& pt);
            
            std::string mVersion;
    };
}

#endif
