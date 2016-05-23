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
        loadCharacterStats(exe);
        loadBaseItems(exe);
        loadUniqueItems(exe);
        loadAffixes(exe);

        FAIO::FAfclose(exe);
    }


    uint32_t DiabloExe::swapEndian(uint32_t arg)
    {

         arg = ((arg << 8) & 0xFF00FF00) | ((arg >> 8) & 0xFF00FF );
         return (arg << 16) | (arg >> 16);

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
                    Npc(exe, name, mSettings.get<size_t>(section, "name"), mSettings.get<size_t>(section, "cel"),
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

    void DiabloExe::loadAffixes(FAIO::FAFile *exe)
    {
        size_t affixOffset = mSettings.get<size_t>("Affix","affixOffset");
        size_t codeOffset = mSettings.get<size_t>("Affix","codeOffset");
        size_t count = mSettings.get<size_t>("Affix","count");


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

    void DiabloExe::loadCharacterStats(FAIO::FAFile *exe)
    {
        size_t startingStatsOffset = mSettings.get<size_t>("CharacterStats", "startingStatsOffset");
        size_t maxStatsOffset = mSettings.get<size_t>("CharacterStats", "maxStatsOffset");
        size_t blockingBonusOffset = mSettings.get<size_t>("CharacterStats", "blockingBonusOffset");
        size_t framesetOffset = mSettings.get<size_t>("CharacterStats", "framesetOffset");
        size_t expPerLevelOffset = mSettings.get<size_t>("CharacterStats", "expPerLevelOffset");
        size_t levelCount = mSettings.get<size_t>("CharacterStats", "maxLevel");
        CharacterStats meleeCharacter, rangerCharacter, mageCharacter;

        FAIO::FAfseek(exe, framesetOffset, SEEK_SET);

        meleeCharacter.mIdleInDungeonFrameset = FAIO::read8(exe);
        rangerCharacter.mIdleInDungeonFrameset = FAIO::read8(exe);
        mageCharacter.mIdleInDungeonFrameset = FAIO::read8(exe);

        meleeCharacter.mAttackFrameset = FAIO::read8(exe);
        rangerCharacter.mAttackFrameset = FAIO::read8(exe);
        mageCharacter.mAttackFrameset = FAIO::read8(exe);

        meleeCharacter.mWalkInDungeonFrameset = FAIO::read8(exe);
        rangerCharacter.mWalkInDungeonFrameset = FAIO::read8(exe);
        mageCharacter.mWalkInDungeonFrameset = FAIO::read8(exe);

        meleeCharacter.mBlockingSpeed = FAIO::read8(exe);
        rangerCharacter.mBlockingSpeed = FAIO::read8(exe);
        mageCharacter.mBlockingSpeed = FAIO::read8(exe);

        meleeCharacter.mDeathFrameset = FAIO::read8(exe);
        rangerCharacter.mDeathFrameset = FAIO::read8(exe);
        mageCharacter.mDeathFrameset = FAIO::read8(exe);

        meleeCharacter.mMagicCastFrameset = FAIO::read8(exe);
        rangerCharacter.mMagicCastFrameset = FAIO::read8(exe);
        mageCharacter.mMagicCastFrameset = FAIO::read8(exe);

        meleeCharacter.mHitRecoverySpeed = FAIO::read8(exe);
        rangerCharacter.mHitRecoverySpeed = FAIO::read8(exe);
        mageCharacter.mHitRecoverySpeed = FAIO::read8(exe);

        meleeCharacter.mIdleInTownFrameset = FAIO::read8(exe);
        rangerCharacter.mIdleInTownFrameset = FAIO::read8(exe);
        mageCharacter.mIdleInTownFrameset = FAIO::read8(exe);

        meleeCharacter.mWalkInTownFrameset = FAIO::read8(exe);
        rangerCharacter.mWalkInTownFrameset = FAIO::read8(exe);
        mageCharacter.mWalkInTownFrameset = FAIO::read8(exe);

        meleeCharacter.mSingleHandedAttackSpeed = FAIO::read8(exe);
        rangerCharacter.mSingleHandedAttackSpeed = FAIO::read8(exe);
        mageCharacter.mSingleHandedAttackSpeed = FAIO::read8(exe);

        meleeCharacter.mSpellCastSpeed = FAIO::read8(exe);
        rangerCharacter.mSpellCastSpeed = FAIO::read8(exe);
        mageCharacter.mSpellCastSpeed = FAIO::read8(exe);

        FAIO::FAfseek(exe, startingStatsOffset, SEEK_SET);

        meleeCharacter.mStrength   = swapEndian(FAIO::read32(exe));
        rangerCharacter.mStrength     = swapEndian(FAIO::read32(exe));
        mageCharacter.mStrength  = swapEndian(FAIO::read32(exe));

        meleeCharacter.mMagic      = swapEndian(FAIO::read32(exe));
        rangerCharacter.mMagic        = swapEndian(FAIO::read32(exe));
        mageCharacter.mMagic     = swapEndian(FAIO::read32(exe));

        meleeCharacter.mDexterity  = swapEndian(FAIO::read32(exe));
        rangerCharacter.mDexterity    = swapEndian(FAIO::read32(exe));
        mageCharacter.mDexterity = swapEndian(FAIO::read32(exe));

        meleeCharacter.mVitality   = swapEndian(FAIO::read32(exe));
        rangerCharacter.mVitality     = swapEndian(FAIO::read32(exe));
        mageCharacter.mVitality  = swapEndian(FAIO::read32(exe));

        FAIO::FAfseek(exe, blockingBonusOffset, SEEK_SET);

        meleeCharacter.mBlockingBonus  = swapEndian(FAIO::read32(exe));
        rangerCharacter.mBlockingBonus    = swapEndian(FAIO::read32(exe));
        mageCharacter.mBlockingBonus = swapEndian(FAIO::read32(exe));

        FAIO::FAfseek(exe, maxStatsOffset, SEEK_SET);

        meleeCharacter.mMaxStrength   = FAIO::read32(exe);
        rangerCharacter.mMaxStrength     = FAIO::read32(exe);
        mageCharacter.mMaxStrength  = FAIO::read32(exe);


        meleeCharacter.mMaxMagic      = FAIO::read32(exe);
        rangerCharacter.mMaxMagic        = FAIO::read32(exe);
        rangerCharacter.mMaxDexterity    = FAIO::read32(exe);

        meleeCharacter.mMaxDexterity  = FAIO::read32(exe);
        mageCharacter.mMaxMagic     = FAIO::read32(exe);
        mageCharacter.mMaxDexterity = FAIO::read32(exe);

        meleeCharacter.mMaxVitality   = FAIO::read32(exe);
        rangerCharacter.mMaxVitality     = FAIO::read32(exe);
        mageCharacter.mMaxVitality  = FAIO::read32(exe);
        FAIO::FAfseek(exe, expPerLevelOffset, SEEK_SET);

        for(uint32_t i=0;i < levelCount; i++)
        {
            uint32_t readLevelData = FAIO::read32(exe);
            meleeCharacter.mNextLevelExp.push_back(readLevelData);
            rangerCharacter.mNextLevelExp.push_back(readLevelData);
            mageCharacter.mNextLevelExp.push_back(readLevelData);
        }

        mCharacters["Warrior"]  = meleeCharacter;
        mCharacters["Rogue"]    = rangerCharacter;
        mCharacters["Sorcerer"] = mageCharacter;
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
            if (levelNum >= it->second.minDunLevel && levelNum <= it->second.maxDunLevel &&
                    it->second.monsterName != "Wyrm" && it->second.monsterName != "Cave Slug" &&
                    it->second.monsterName != "Devil Wyrm" && it->second.monsterName != "Devourer") // Exception, these monster's CEL files don't exist
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
