#include "diabloexe.h"

#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <iostream>

#include <misc/md5.h>
#include <misc/stringops.h>

namespace DiabloExe
{
    DiabloExe::DiabloExe(const std::string& pathEXE)
    {
        mVersion = getVersion(pathEXE);
        if (mVersion.empty())
        {
            return;
        }

        if(!mSettings.loadFromFile("resources/exeversions/" + mVersion + ".ini"))
        {
            std::cout << "Cannot load settings file.";
            return;
        }
        
        FAIO::FAFile* exe = FAIO::FAfopen(pathEXE);
        if (NULL == exe)
        {
            return;
        }

        loadMonsters(exe);
        loadNpcs(exe);
        loadBaseItems(exe);
        loadUniqueItems(exe);
        loadPreficies(exe);

        FAIO::FAfclose(exe);
    }

    std::string DiabloExe::getMD5(const std::string& pathEXE)
    {
        FAIO::FAFile* dexe = FAIO::FAfopen(pathEXE);
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

    std::string DiabloExe::getVersion(const std::string& pathEXE)
    {
        std::string exeMD5 = getMD5(pathEXE);
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

            if(mMonsters.find(tmp.monsterName) != mMonsters.end())
            {
                size_t i;
                for(i = 1; mMonsters.find(tmp.monsterName + "_" + std::to_string(i)) != mMonsters.end(); i++);

                mMonsters[tmp.monsterName + "_" + std::to_string(i)] = tmp;
            }
            else
            {
                mMonsters[tmp.monsterName] = tmp;
            }
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

    void DiabloExe::loadBaseItems(FAIO::FAFile *exe)
    {
        size_t itemOffset = mSettings.get<size_t>("BaseItems","itemOffset");
        size_t codeOffset = mSettings.get<size_t>("BaseItems","codeOffset");
        size_t count = mSettings.get<size_t>("BaseItems","count");

        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, itemOffset + 76*i, SEEK_SET);
            BaseItem tmp(exe, codeOffset);
            
            if(tmp.useOnce > 1 || tmp.itemName == "")
                continue;
            if(Misc::StringUtils::containsNonPrint(tmp.itemName))
                continue;
            if(Misc::StringUtils::containsNonPrint(tmp.itemSecondName))
                continue;
            if(mBaseItems.find(tmp.itemName) != mBaseItems.end())
            {
                size_t i;
                for(i = 1; mBaseItems.find(tmp.itemName + "_" + std::to_string(i)) != mBaseItems.end(); i++);

                mBaseItems[tmp.itemName + "_" + std::to_string(i)] = tmp;
            }
            else
            {
                mBaseItems[tmp.itemName] = tmp;
            }
        }
    }
    void DiabloExe::loadUniqueItems(FAIO::FAFile *exe)
    {
        size_t itemOffset = mSettings.get<size_t>("UniqueItems","uniqueItemOffset");
        size_t codeOffset = mSettings.get<size_t>("UniqueItems","codeOffset");
        size_t count = mSettings.get<size_t>("UniqueItems","count");

        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, itemOffset + 76*i, SEEK_SET);
            UniqueItem tmp(exe, codeOffset);
            if
                    (
                    tmp.mEffect0 > 79 ||
                    tmp.mEffect1 > 79 ||
                    tmp.mEffect2 > 79 ||
                    tmp.mEffect3 > 79 ||
                    tmp.mEffect4 > 79 ||
                    tmp.mEffect5 > 79
                    )
                continue;
            if (tmp.mItemType == 0 || tmp.mItemType > 68)
                continue;
            uint32_t maxRange = 50000;
            if
                    (
                     tmp.mMaxRange0 > maxRange ||
                     tmp.mMaxRange1 > maxRange ||
                     tmp.mMaxRange2 > maxRange ||
                     tmp.mMaxRange3 > maxRange ||
                     tmp.mMaxRange4 > maxRange ||
                     tmp.mMaxRange5 > maxRange ||
                     tmp.mMinRange0 > maxRange ||
                     tmp.mMinRange1 > maxRange ||
                     tmp.mMinRange2 > maxRange ||
                     tmp.mMinRange3 > maxRange ||
                     tmp.mMinRange4 > maxRange ||
                     tmp.mMinRange5 > maxRange
                     )
                continue;

            if(Misc::StringUtils::containsNonPrint(tmp.mName))
                continue;




            if(mUniqueItems.find(tmp.mName) != mUniqueItems.end())
            {
                size_t i;
                for(i = 1; mUniqueItems.find(tmp.mName + "_" + std::to_string(i)) != mUniqueItems.end(); i++);

                mUniqueItems[tmp.mName + "_" + std::to_string(i)] = tmp;
            }
            else
            {
                mUniqueItems[tmp.mName] = tmp;
            }
        }
    }

    void DiabloExe::loadPreficies(FAIO::FAFile *exe)
    {
        size_t prefixOffset = mSettings.get<size_t>("Preficies","prefixOffset");
        size_t codeOffset = mSettings.get<size_t>("Preficies","codeOffset");
        size_t count = mSettings.get<size_t>("Preficies","count");

        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, prefixOffset + 48*i, SEEK_SET);
            Prefix tmp(exe, codeOffset);

            if(mPrefices.find(tmp.prefixName) != mPrefices.end())
            {
                size_t i;
                for(i = 1; mPrefices.find(tmp.prefixName + "_" + std::to_string(i)) != mPrefices.end(); i++);

                mPrefices[tmp.prefixName + "_" + std::to_string(i)] = tmp;
            }
            else
            {
                mPrefices[tmp.prefixName] = tmp;
            }
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

        ss << "Base Items: "<< mBaseItems.size() << std::endl;
        for(std::map<std::string, BaseItem>::const_iterator it = mBaseItems.begin(); it != mBaseItems.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        ss << "Unique Items: "<< mUniqueItems.size() << std::endl;
        for(std::map<std::string, UniqueItem>::const_iterator it = mUniqueItems.begin(); it != mUniqueItems.end(); ++it)
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
