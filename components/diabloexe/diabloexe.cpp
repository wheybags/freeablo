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
        loadCharacterStats(exe, pt);
        loadBaseItems(exe, pt);
        loadUniqueItems(exe, pt);
        loadAffixes(exe, pt);

        FAIO::FAfclose(exe);
    }

    uint32_t DiabloExe::swapEndian(uint32_t arg)
    {

         arg = ((arg << 8) & 0xFF00FF00) | ((arg >> 8) & 0xFF00FF );
         return (arg << 16) | (arg >> 16);

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
    void DiabloExe::loadUniqueItems(FAIO::FAFile *exe, boost::property_tree::ptree &pt)
    {
        size_t itemOffset = pt.get<size_t>("UniqueItems.uniqueItemOffset");
        size_t codeOffset = pt.get<size_t>("UniqueItems.codeOffset");
        size_t count = pt.get<size_t>("UniqueItems.count");
        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, itemOffset + 84*i, SEEK_SET);
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

    void DiabloExe::loadAffixes(FAIO::FAFile *exe, boost::property_tree::ptree &pt)
    {
        size_t affixOffset = pt.get<size_t>("Affix.affixOffset");
        size_t codeOffset = pt.get<size_t>("Affix.codeOffset");
        size_t count = pt.get<size_t>("Affix.count");
        for(size_t i=0; i < count; i++)
        {
            FAIO::FAfseek(exe, affixOffset + 48*i, SEEK_SET);
            Affix tmp(exe, codeOffset);
            if(Misc::StringUtils::containsNonPrint(tmp.mName))
                continue;
            if(tmp.mName.empty())
                continue;
            if(mAffixes.find(tmp.mName) != mAffixes.end())
            {
                size_t i;
                for(i = 1; mAffixes.find(tmp.mName + "_" + std::to_string(i)) != mAffixes.end(); i++);

                mAffixes[tmp.mName + "_" + std::to_string(i)] = tmp;
            }
            else
            {
                mAffixes[tmp.mName] = tmp;
            }
        }
    }

    void DiabloExe::loadCharacterStats(FAIO::FAFile *exe, boost::property_tree::ptree &pt)
    {
        size_t startingStatsOffset = pt.get<size_t>("CharacterStats.startingStatsOffset");
        size_t maxStatsOffset = pt.get<size_t>("CharacterStats.maxStatsOffset");
        size_t blockingBonusOffset = pt.get<size_t>("CharacterStats.blockingBonusOffset");
        CharacterStats warrior, rogue, sorcerer;

        FAIO::FAfseek(exe, startingStatsOffset-6, SEEK_SET);

        warrior.mStrength   = swapEndian(FAIO::read32(exe));
        rogue.mStrength     = swapEndian(FAIO::read32(exe));
        sorcerer.mStrength  = swapEndian(FAIO::read32(exe));

        warrior.mMagic      = swapEndian(FAIO::read32(exe));
        rogue.mMagic        = swapEndian(FAIO::read32(exe));
        sorcerer.mMagic     = swapEndian(FAIO::read32(exe));

        warrior.mDexterity  = swapEndian(FAIO::read32(exe));
        rogue.mDexterity    = swapEndian(FAIO::read32(exe));
        sorcerer.mDexterity = swapEndian(FAIO::read32(exe));

        warrior.mVitality   = swapEndian(FAIO::read32(exe));
        rogue.mVitality     = swapEndian(FAIO::read32(exe));
        sorcerer.mVitality  = swapEndian(FAIO::read32(exe));

        FAIO::FAfseek(exe, SEEK_SET, blockingBonusOffset);

        warrior.mBlockingBonus  = swapEndian(FAIO::read32(exe));
        rogue.mBlockingBonus    = swapEndian(FAIO::read32(exe));
        sorcerer.mBlockingBonus = swapEndian(FAIO::read32(exe));

        FAIO::FAfseek(exe, maxStatsOffset, SEEK_SET);

        warrior.mMaxStrength   = FAIO::read32(exe);
        rogue.mMaxStrength     = FAIO::read32(exe);
        sorcerer.mMaxStrength  = FAIO::read32(exe);


        warrior.mMaxMagic      = FAIO::read32(exe);
        rogue.mMaxMagic        = FAIO::read32(exe);
        rogue.mMaxDexterity    = FAIO::read32(exe);

        warrior.mMaxDexterity  = FAIO::read32(exe);
        sorcerer.mMaxMagic     = FAIO::read32(exe);
        sorcerer.mMaxDexterity = FAIO::read32(exe);

        warrior.mMaxVitality   = FAIO::read32(exe);
        rogue.mMaxVitality     = FAIO::read32(exe);
        sorcerer.mMaxVitality  = FAIO::read32(exe);

        mCharacters["Warrior"]  = warrior;
        mCharacters["Rogue"]    = rogue;
        mCharacters["Sorcerer"] = sorcerer;
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

    const std::map<std::string, UniqueItem> & DiabloExe::getUniqueItemMap() const
    {
        return mUniqueItems;
    }

    const CharacterStats DiabloExe::getCharacterStat(std::string character) const
    {
        return mCharacters.at(character);

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

        ss << "Character Stats: " << mCharacters.size() << std::endl
           << "Warrior" << std::endl
           << mCharacters.at("Warrior").dump()
           << "Rogue" << std::endl
           << mCharacters.at("Rogue").dump()
           << "Sorcerer" << std::endl
           << mCharacters.at("Sorcerer").dump();


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

        ss << "Affixes: " << mAffixes.size() << std::endl;
        for(std::map<std::string, Affix>::const_iterator it = mAffixes.begin(); it != mAffixes.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        return ss.str();
    }

    bool DiabloExe::isLoaded() const
    {
        return !mMonsters.empty() && !mNpcs.empty() && !mBaseItems.empty() && !mAffixes.empty();
    }
}
