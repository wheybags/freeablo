#pragma once
#include "../../apps/freeablo/faworld/itemenums.h"
#include <array>
#include <faio/fafileobject.h>
#include <map>
#include <memory>
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

    class MissileGraphics
    {
    public:
        uint8_t mNumAnimationFiles;
        std::string mFilename;
        uint32_t mFlags;
        int32_t mAnimationDelays[16];
    };

    class MissileData
    {
    public:
        bool mDraw;
        uint8_t mType;
        uint8_t mResist;
        uint8_t mMissileGraphicsId;
        std::string mSoundEffect;
        std::string mImpactSoundEffect;
    };

    class SpellData
    {
    public:
        enum class SpellType
        {
            fire,
            lightning,
            magic
        };

        int32_t mManaCost;
        SpellType mType;
        std::string mNameText;
        std::string mSkillText;
        int32_t mBookLvl;
        int32_t mStaffLvl;
        bool mTargeted;
        bool mTownSpell;
        int32_t mMinMagic;
        std::string mSoundEffect;
        int32_t mMissiles[3];
        int32_t mManaAdj;
        int32_t mMinMana;
        int32_t mStaffMin;
        int32_t mStaffMax;
        int32_t mBookCost;
        int32_t mStaffCost;
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
        const std::map<uint8_t, MissileGraphics>& getMissileGraphicsTable() const { return mMissileGraphicsTable; }
        const std::map<uint8_t, MissileData>& getMissileDataTable() const { return mMissileDataTable; }
        const std::map<uint8_t, SpellData>& getSpellsDataTable() const { return mSpellsDataTable; }

        struct VersionResult
        {
            std::string version;
            std::string iniPath;

            bool empty() const { return version.empty(); }
        };
        static VersionResult getVersion(const std::string& pathEXE);

    private:
        static std::string getMD5(const std::string& pathEXE);

        void loadDropGraphicsFilenames(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadSoundFilenames(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadMonsters(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadNpcs(FAIO::FAFileObject& exe);
        void loadBaseItems(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadUniqueItems(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadAffixes(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadCharacterStats(FAIO::FAFileObject& exe);
        void loadTownerAnimation(FAIO::FAFileObject& exe);
        void loadMissileGraphicsTable(FAIO::FAFileObject& exe, size_t codeOffset);
        void loadMissileDataTable(FAIO::FAFileObject& exe);
        void loadSpellsTable(FAIO::FAFileObject& exe, size_t codeOffset);

        std::unique_ptr<Settings::Settings> mSettings;

        VersionResult mVersion;
        std::map<std::string, Monster> mMonsters;
        std::map<std::string, Npc> mNpcs;
        std::map<std::string, CharacterStats> mCharacters;
        std::vector<BaseItem> mBaseItems;
        std::vector<UniqueItem> mUniqueItems;
        std::vector<Affix> mAffixes;
        std::vector<std::vector<int32_t>> mTownerAnimation;
        std::vector<std::string> mItemDropGraphicsFilename;
        std::vector<std::string> mSoundFilename;
        std::vector<uint32_t> mItemGraphicsIdToDropSfxId;
        std::vector<uint32_t> mItemGraphicsIdToInvPlaceSfxId;
        std::unordered_map<std::string, FontData> mFontData;
        std::map<uint8_t, MissileGraphics> mMissileGraphicsTable;
        std::map<uint8_t, MissileData> mMissileDataTable;
        std::map<uint8_t, SpellData> mSpellsDataTable;
    };
}
