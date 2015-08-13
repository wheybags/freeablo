#include "diabloexe.h"

#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <iostream>

#include <misc/md5.h>
#include <misc/stringops.h>

namespace DiabloExe
{
    namespace bpt = boost::property_tree;

    DiabloExe::DiabloExe()
    {
        mVersion = getVersion();
        if (mVersion.empty())
        {
            return;
        }

        if(!mSettings.loadFromFile("resources/exeversions/" + mVersion + ".ini"))
        {
            std::cout << "Cannot load settings file.";
            return;
        }
        
        FAIO::FAFile* exe = FAIO::FAfopen("Diablo.exe");
        if (NULL == exe)
        {
            return;
        }

        loadMonsters(exe);
        loadNpcs(exe);

        FAIO::FAfclose(exe);
    }

    std::string DiabloExe::getMD5()
    {
        FAIO::FAFile* dexe = FAIO::FAfopen("Diablo.exe");
        if (NULL == dexe)
        {
            return std::string();
        }

        size_t size = FAIO::FAsize(dexe);
        Misc::md5_byte_t* buff = new Misc::md5_byte_t[size];
        FAIO::FAfread(buff, sizeof(Misc::md5_byte_t), size, dexe);
        
        Misc::md5_state_t state;
        Misc::md5_byte_t digest[16];

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
        std::string exeMD5 = getMD5();
        if (exeMD5.empty())
        {
            return std::string();
        }

        Settings::Settings settings;
        std::string version = "";
        settings.loadFromFile("resources/exeversions/versions.ini");
        Settings::Container sections = settings.getSections();

        for(Settings::Container::iterator it = sections.begin(); it != sections.end(); ++it)
        {
            std::string temporaryVersion = settings.get<std::string>("",*it);
            if(temporaryVersion == exeMD5)
            {
                version = *it;
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

    void DiabloExe::loadMonsters(FAIO::FAFile* exe)
    {
        size_t monsterOffset = mSettings.get<size_t>("Monsters", "monsterOffset");
        size_t codeOffset = mSettings.get<size_t>("Monsters", "codeOffset");
        size_t count = mSettings.get<size_t>("Monsters", "count");

        for(size_t i = 0; i < count; i++)
        {
            FAIO::FAfseek(exe, monsterOffset + 128*i, SEEK_SET);

            Monster tmp(exe, codeOffset);
            mMonsters[tmp.monsterName] = tmp;
        }
    }
    
    void DiabloExe::loadNpcs(FAIO::FAFile* exe)
    {
        Settings::Container sections = mSettings.getSections();

        for(Settings::Container::const_iterator it = sections.begin(); it != sections.end(); ++it)
        {
            std::string name = *it;
            std::string section = name;

            if(Misc::StringUtils::startsWith(name, "NPC"))
            {
                mNpcs[name.substr(3, name.size()-3)] =
                    Npc(exe, mSettings.get<size_t>(section, "name"), mSettings.get<size_t>(section, "cel"),
                        mSettings.get<size_t>(section, "x"), mSettings.get<size_t>(section, "y"), mSettings.get<size_t>(section, "rotation", 0));
            }
        }
    }

    const Monster& DiabloExe::getMonster(const std::string& name) const
    {
        return mMonsters.find(name)->second;
    }

    std::vector<const Monster*> DiabloExe::getMonstersInLevel(size_t levelNum) const
    {
        std::vector<const Monster*> retval;

        for(std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            if(levelNum >= it->second.minDunLevel && levelNum <= it->second.maxDunLevel &&
               it->second.monsterName != "Wyrm" && it->second.monsterName != "Cave Slug") // Exception, these monster's CEL files don't exist
            {
                retval.push_back(&(it->second));
            }
        }

        return retval;
    }

    const Npc& DiabloExe::getNpc(const std::string& name) const
    {
        return mNpcs.find(name)->second;
    }

    std::vector<const Npc*> DiabloExe::getNpcs() const
    {
        std::vector<const Npc*> retval;

        for(std::map<std::string, Npc>::const_iterator it = mNpcs.begin(); it != mNpcs.end(); ++it)
            retval.push_back(&(it->second));

        return retval;
    }

    std::string DiabloExe::dump() const
    {
        std::stringstream ss;
        
        ss << "Monsters:" << std::endl;
        for(std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            ss << it->second.dump();
        }
        
        ss << "Npcs:" << std::endl;
        for(std::map<std::string, Npc>::const_iterator it = mNpcs.begin(); it != mNpcs.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        return ss.str();
    }

    bool DiabloExe::isLoaded() const
    {
        return !mMonsters.empty() && !mNpcs.empty();
    }
}
