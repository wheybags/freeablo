#ifndef DIABLO_EXE_H
#define DIABLO_EXE_H

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <faio/fafileobject.h>

namespace Settings
{
    class Settings;
}

namespace DiabloExe
{
    class Monster;
    class Npc;
    class BaseItem;
    class CharacterStats;
    class UniqueItem;
    class Affix;

    class DiabloExe
    {
        public:

            DiabloExe(const std::string& pathEXE = "Diablo.exe");
            ~DiabloExe ();

            const Monster& getMonster(const std::string& name) const;
            std::vector<const Monster*> getMonstersInLevel(size_t levelNum) const;

            const Npc& getNpc(const std::string& name) const;
            std::vector<const Npc*> getNpcs() const;
            const std::vector<std::vector<int32_t>> &getTownerAnimation() const;

            const BaseItem& getItem(const std::string& name) const;
            std::map<std::string, BaseItem> getItemMap() const;
            const std::map<std::string, UniqueItem> & getUniqueItemMap() const;
            const CharacterStats getCharacterStat(std::string character) const;

            std::string dump() const;

            bool isLoaded() const;

            uint32_t swapEndian(uint32_t arg);



        private:
            std::string getMD5(const std::string& pathEXE);
            std::string getVersion(const std::string& pathEXE);


            void loadDropGraphicsFilenames(FAIO::FAFileObject& exe, size_t codeOffset);
            void loadMonsters(FAIO::FAFileObject& exe, size_t codeOffset);
            void loadNpcs(FAIO::FAFileObject& exe);
            void loadBaseItems(FAIO::FAFileObject& exe, size_t codeOffset);
            void loadUniqueItems(FAIO::FAFileObject& exe, size_t codeOffset);
            void loadAffixes(FAIO::FAFileObject& exe, size_t codeOffset);
            void loadCharacterStats(FAIO::FAFileObject& exe);
            void loadTownerAnimation(FAIO::FAFileObject& exe);

            std::unique_ptr<Settings::Settings> mSettings;

            std::string mVersion;
            std::map<std::string, Monster> mMonsters;
            std::map<std::string, Npc> mNpcs;
            std::map<std::string, BaseItem> mBaseItems;
            std::map<std::string, UniqueItem> mUniqueItems;
            std::map<std::string, Affix> mAffixes;
            std::map<std::string, CharacterStats> mCharacters;
            std::vector<std::vector<int32_t>> mTownerAnimation;
            std::vector<std::string> itemDropGraphicsFilename;
    };
}

#endif
