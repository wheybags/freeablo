
#pragma once

#include <array>
#include <map>
#include <memory>

#include <faio/fafileobject.h>
#include <unordered_map>

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

    class FontData
    {
        // Basic logic on how fonts work:
        // charToFontIndex maps normal char 0..255 to font index 0..127 similar to ASCII indices
        // font index could be then transformed to frame by fontIndexToFrame array specific to font
        // frame then could be used for array frameToWidth to extract width or draw
        constexpr static int charSize = 256;
        constexpr static int fontIndexSize = 128;

    public:
        std::array<uint8_t, charSize> charToFontIndex;
        std::array<uint8_t, fontIndexSize> fontIndexToFrame;
        std::vector<uint8_t> frameToWidth;
        int frameCount;
    };

    class DiabloExe
    {
    public:
        void loadFontData(FAIO::FAFileObject& exe);
        DiabloExe(const std::string& pathEXE = "Diablo.exe");
        ~DiabloExe();

        const Monster& getMonster(const std::string& name) const;
        std::vector<const Monster*> getMonstersInLevel(size_t levelNum) const;

        const Npc& getNpc(const std::string& name) const;
        std::vector<const Npc*> getNpcs() const;
        const std::vector<std::vector<int32_t>>& getTownerAnimation() const;

        const CharacterStats getCharacterStat(std::string character) const;

        std::string dump() const;

        bool isLoaded() const;

        uint32_t swapEndian(uint32_t arg);
        const FontData& getFontData(const char* fontName) const;
        const std::vector<BaseItem>& getBaseItems() const { return mBaseItems; }
        const std::vector<UniqueItem>& getUniqueItems() const { return mUniqueItems; }
        const std::vector<Affix>& getAffixes() const { return mAffixes; }

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
        std::map<std::string, CharacterStats> mCharacters;
        std::vector<BaseItem> mBaseItems;
        std::vector<UniqueItem> mUniqueItems;
        std::vector<Affix> mAffixes;
        std::vector<std::vector<int32_t>> mTownerAnimation;
        std::vector<std::string> itemDropGraphicsFilename;
        std::unordered_map<std::string, FontData> mFontData;
    };
}
