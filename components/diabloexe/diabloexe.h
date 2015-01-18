#ifndef DIABLO_EXE_H
#define DIABLO_EXE_H

#include <string>
#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <faio/faio.h>

#include "monster.h"
#include "npc.h"

namespace DiabloExe
{
    class DiabloExe
    {
        public:
            DiabloExe();

            const Monster& getMonster(const std::string& name) const;
            std::vector<const Monster*> getMonstersInLevel(size_t levelNum) const;

            const Npc& getNpc(const std::string& name) const;
            std::vector<const Npc*> getNpcs() const;

            std::string dump() const;

            bool isLoaded() const;

        private:
            std::string getMD5();
            std::string getVersion();

            void loadMonsters(FAIO::FAFile* exe, boost::property_tree::ptree& pt);
            void loadNpcs(FAIO::FAFile* exe, boost::property_tree::ptree& pt);
            
            std::string mVersion;
            std::map<std::string, Monster> mMonsters;
            std::map<std::string, Npc> mNpcs;
    };
}

#endif
