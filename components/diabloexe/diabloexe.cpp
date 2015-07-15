#include "diabloexe.h"

#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <iostream>

#include <misc/fareadini.h>
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

        bpt::ptree pt;
        Misc::readIni("resources/exeversions/" + mVersion + ".ini", pt);
        
        FAIO::FAFile* exe = FAIO::FAfopen("Diablo.exe");
        if (NULL == exe)
        {
            return;
        }

        loadMonsters(exe, pt);                        
        loadNpcs(exe, pt);
        loadBaseItems(exe, pt);
        loadPreficies(exe, pt);
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
        bpt::ptree pt;
        Misc::readIni("resources/exeversions/versions.ini", pt);

        std::string exeMD5 = getMD5();
        if (exeMD5.empty())
        {
            return std::string();
        }

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
    
    void DiabloExe::loadNpcs(FAIO::FAFile* exe, boost::property_tree::ptree& pt)
    {
        for(bpt::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it)
        {
            if(Misc::StringUtils::startsWith(it->first, "NPC"))
            {
                mNpcs[it->first.substr(3, it->first.size()-3)] =
                    Npc(exe, it->second.get<size_t>("name"), it->second.get<size_t>("cel"),
                        it->second.get<size_t>("x"), it->second.get<size_t>("y"), it->second.get("rotation", 0));
            }
        }
    }

    void DiabloExe::loadBaseItems(FAIO::FAFile *exe, boost::property_tree::ptree &pt)
    {
        size_t itemOffset = pt.get<size_t>("BaseItems.itemOffset");
        size_t codeOffset = pt.get<size_t>("BaseItems.codeOffset");
        size_t count = pt.get<size_t>("BaseItems.count");
        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, itemOffset + 76*i, SEEK_SET);
            BaseItem tmp(exe, codeOffset);
            mBaseItems[tmp.itemName] = tmp;

        }

    }
    void DiabloExe::loadPreficies(FAIO::FAFile *exe, boost::property_tree::ptree &pt)
    {
        size_t prefixOffset = pt.get<size_t>("Preficies.prefixOffset");
        size_t codeOffset = pt.get<size_t>("Preficies.codeOffset");
        size_t count = pt.get<size_t>("Preficies.count");
        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, prefixOffset + 48*i, SEEK_SET);
            Prefix tmp(exe, codeOffset);
            mPrefices[tmp.prefixName] = tmp;

        }

    }

    const Monster& DiabloExe::getMonster(const std::string& name) const
    {
        return mMonsters.find(name)->second;
    }

    const BaseItem& DiabloExe::getItem(const std::string &name) const
    {
        return mBaseItems.find(name)->second;

    }
    std::map<std::string, BaseItem> DiabloExe::getItemMap() const
    {
        return mBaseItems;

    }

    std::vector<const Monster*> DiabloExe::getMonstersInLevel(size_t levelNum) const
    {
        std::vector<const Monster*> retval;

        for(std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            if(levelNum >= it->second.minDunLevel && levelNum <= it->second.maxDunLevel)
                retval.push_back(&(it->second));
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
        
        ss << "Monsters: " << mMonsters.size() << std::endl;
        for(std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            ss << it->second.dump();
        }
        
        ss << "Npcs: " << mNpcs.size() << std::endl;
        for(std::map<std::string, Npc>::const_iterator it = mNpcs.begin(); it != mNpcs.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        ss << "Items: "<< mBaseItems.size() << std::endl;
        for(std::map<std::string, BaseItem>::const_iterator it = mBaseItems.begin(); it != mBaseItems.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        ss << "Prefices: " << mPrefices.size() << std::endl;
        for(std::map<std::string, Prefix>::const_iterator it = mPrefices.begin(); it != mPrefices.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        return ss.str();
    }

    bool DiabloExe::isLoaded() const
    {
        return !mMonsters.empty() && !mNpcs.empty() && !mBaseItems.empty() && !mPrefices.empty();
    }
}
