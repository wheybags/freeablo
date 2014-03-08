#include "diabloexe.h"

#include <stdint.h>

#include <iomanip>
#include <sstream>

#include <misc/fareadini.h>
#include <misc/md5.h>

namespace DiabloExe
{
    namespace bpt = boost::property_tree;

    DiabloExe::DiabloExe()
    {
        mVersion = getVersion();

        bpt::ptree pt;
        Misc::readIni("resources/exeversions/" + mVersion + ".ini", pt);
        
        FAIO::FAFile* exe = FAIO::FAfopen("Diablo.exe");
        loadMonsters(exe, pt);                        

        FAIO::FAfclose(exe);
    }

    std::string DiabloExe::getMD5()
    {
        FAIO::FAFile* dexe = FAIO::FAfopen("Diablo.exe");
        size_t size = FAIO::FAsize(dexe);
        Misc::md5_byte_t* buff = new Misc::md5_byte_t[size];
        FAIO::FAfread(buff, sizeof(Misc::md5_byte_t), size, dexe);
        
        Misc::md5_state_t state;
        Misc::md5_byte_t digest[16];
        int di;

        md5_init(&state);
        md5_append(&state, buff, size);
        md5_finish(&state, digest);

        delete[] buff;
        FAIO::FAfclose(dexe);

        std::stringstream s;

        for(size_t i = 0; i < 16; i++)
           s << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];

        return s.str();
    }

    std::string DiabloExe::getVersion()
    {
        bpt::ptree pt;
        Misc::readIni("resources/exeversions/versions.ini", pt);

        std::string exeMD5 = getMD5();

        std::string version = "";

        for(bpt::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it)
        {
            if(it->second.get_value<std::string>() == exeMD5)
            {
                version = it->first;
                break;
            }
        }

        if(version == "")
        {
            std::cerr << "Unrecognised version of Diablo.exe" << std::endl;
            return "";
        }

        else
            std::cout << "Diablo.exe " << version << " detected" << std::endl;
        
        return version;
    }

    void DiabloExe::loadMonsters(FAIO::FAFile* exe, bpt::ptree& pt)
    {
        size_t monsterOffset = pt.get<size_t>("Monsters.monsterOffset"); 
        size_t codeOffset = pt.get<size_t>("Monsters.codeOffset"); 
        size_t count = pt.get<size_t>("Monsters.count");

        for(size_t i = 0; i < count; i++)
        {
            FAIO::FAfseek(exe, monsterOffset + 128*i, SEEK_SET);

            Monster tmp(exe, codeOffset);
            mMonsters[tmp.monsterName] = tmp;
        }
    }

    const Monster& DiabloExe::getMonster(const std::string& name) const
    {
        return mMonsters.find(name)->second;
    }

    std::string DiabloExe::dump() const
    {
        std::stringstream ss;

        for(std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            ss << it->second.dump();
        }

        return ss.str();
    }
}
