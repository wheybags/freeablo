#include "diabloexe.h"
#include "../../apps/freeablo/faworld/item.h"
#include "baseitem.h"
#include "characterstats.h"
#include "monster.h"
#include "npc.h"
#include "settings/settings.h"
#include "talkdata.h"
#include <iomanip>
#include <iostream>
#include <misc/md5.h>
#include <misc/misc.h>
#include <misc/stringops.h>
#include <sstream>
#include <stdint.h>

namespace DiabloExe
{
    void DiabloExe::loadFontData(FAIO::FAFileObject& exe)
    {
        std::array<uint8_t, 256> charToFontIndex;
        exe.FAfseek(mSettings->get<size_t>("Fonts", "charToFontIndex"), SEEK_SET);
        exe.FAfread(charToFontIndex.data(), 1, charToFontIndex.size());
        for (std::string fontName : {"smaltext", "bigtgold"})
        {
            auto& data = mFontData[fontName];
            data.charToFontIndex = charToFontIndex;
            exe.FAfseek(mSettings->get<size_t>("Fonts", fontName + "FontIndexToFrame"), SEEK_SET);
            exe.FAfread(data.fontIndexToFrame.data(), 1, data.fontIndexToFrame.size());

            data.frameCount = mSettings->get<size_t>("Fonts", fontName + "FrameCount");
            data.frameToWidth.resize(data.frameCount + 1);
            exe.FAfseek(mSettings->get<size_t>("Fonts", fontName + "FrameToWidth"), SEEK_SET);
            exe.FAfread(data.frameToWidth.data(), 1, data.frameToWidth.size());
        }
    }

    DiabloExe::DiabloExe(const std::string& pathEXE)
    {
        if (pathEXE.empty())
            return;

        mSettings.reset(new Settings::Settings());
        mVersion = getVersion(pathEXE);
        if (mVersion.empty())
            return;

        if (!mSettings->loadFromFile(mVersion.iniPath))
        {
            std::cout << "Cannot load settings file.";
            return;
        }

        FAIO::FAFileObject exe(pathEXE);
        if (!exe.isValid())
            return;

        auto codeOffset = mSettings->get<size_t>("Common", "codeOffset");
        loadFontData(exe);
        loadMonsters(exe, codeOffset);
        loadTownerAnimation(exe);
        loadNpcs(exe);
        loadCharacterStats(exe);
        loadDropGraphicsFilenames(exe, codeOffset);
        loadSoundFilenames(exe, codeOffset);
        loadBaseItems(exe, codeOffset);
        loadUniqueItems(exe, codeOffset);
        loadAffixes(exe, codeOffset);
        loadMissileGraphicsTable(exe, codeOffset);
        loadMissileDataTable(exe);
        loadSpellsTable(exe, codeOffset);
    }

    DiabloExe::~DiabloExe() {}

    uint32_t DiabloExe::swapEndian(uint32_t arg)
    {

        arg = ((arg << 8) & 0xFF00FF00) | ((arg >> 8) & 0xFF00FF);
        return (arg << 16) | (arg >> 16);
    }

    const FontData& DiabloExe::getFontData(const char* fontName) const { return mFontData.at(fontName); }

    std::string DiabloExe::getMD5(const std::string& pathEXE)
    {
        FAIO::FAFileObject dexe(pathEXE);
        if (!dexe.isValid())
        {
            return std::string();
        }

        size_t size = dexe.FAsize();
        Misc::md5_byte_t* buff = new Misc::md5_byte_t[size];
        dexe.FAfread(buff, sizeof(Misc::md5_byte_t), size);

        Misc::md5_state_t state;
        Misc::md5_byte_t digest[16];

        md5_init(&state);
        md5_append(&state, buff, size);
        md5_finish(&state, digest);

        delete[] buff;

        std::stringstream s;

        for (size_t i = 0; i < 16; i++)
            s << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];

        return s.str();
    }

    DiabloExe::VersionResult DiabloExe::getVersion(const std::string& pathEXE)
    {
        std::string exeMD5 = getMD5(pathEXE);
        if (exeMD5.empty())
            return {"", ""};

        Settings::Settings settings;
        std::string version = "";
        settings.loadFromFile(Misc::getResourcesPath().str() + "/exeversions/versions.ini");
        const Settings::Container versionProperties = settings.getPropertiesInSection("");

        for (const auto& versionProperty : versionProperties)
        {
            const std::string temporaryVersion = settings.get<std::string>("", versionProperty);
            if (temporaryVersion == exeMD5)
            {
                version = versionProperty;
                break;
            }
        }

        if (version == "")
        {
            std::cerr << "Unrecognised version of Diablo.exe" << std::endl;
            return {"", ""};
        }

        else
            std::cout << "Diablo.exe " << version << " detected" << std::endl;

        std::string iniPath = Misc::getResourcesPath().str() + "/exeversions/" + settings.get<std::string>("", "ini_" + version, version + ".ini");

        return {version, iniPath};
    }

    void DiabloExe::loadDropGraphicsFilenames(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        const uint64_t offset = mSettings->get<uint64_t>("ItemDropGraphics", "filenames");
        mItemDropGraphicsFilename.resize(35);
        for (int i = 0; i < static_cast<int>(mItemDropGraphicsFilename.size()); ++i)
        {
            exe.FAfseek(offset + i * 4, SEEK_SET);
            auto nameOffset = exe.read32();
            mItemDropGraphicsFilename[i] = exe.readCStringFromWin32Binary(nameOffset, codeOffset);
        }
    }

    void DiabloExe::loadSoundFilenames(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        uint64_t offset = mSettings->get<uint64_t>("Sounds", "filenameTable");
        int32_t filenameTableSize = mSettings->get<int32_t>("Sounds", "filenameTableSize");
        mSoundFilename.resize(filenameTableSize);
        for (int i = 0; i < static_cast<int>(mSoundFilename.size()); ++i)
        {
            exe.FAfseek(offset + i * 9 + 1, SEEK_SET);
            auto nameOffset = exe.read32();
            mSoundFilename[i] = exe.readCStringFromWin32Binary(nameOffset, codeOffset);
            mSoundFilename[i] = Misc::StringUtils::toLower(mSoundFilename[i]);
            Misc::StringUtils::replace(mSoundFilename[i], "\\", "/");
        }

        offset = mSettings->get<uint64_t>("Sounds", "itemGraphicsIdToDropSfxId");
        mItemGraphicsIdToDropSfxId.resize(35);
        exe.FAfseek(offset, SEEK_SET);
        for (auto& sfxLookup : mItemGraphicsIdToDropSfxId)
            sfxLookup = exe.read32();

        offset = mSettings->get<uint64_t>("Sounds", "itemGraphicsIdToInvPlaceSfxId");
        mItemGraphicsIdToInvPlaceSfxId.resize(35);
        exe.FAfseek(offset, SEEK_SET);
        for (auto& sfxLookup : mItemGraphicsIdToInvPlaceSfxId)
            sfxLookup = exe.read32();
    }

    void DiabloExe::loadMonsters(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        size_t monsterOffset = mSettings->get<size_t>("Monsters", "monsterOffset");
        size_t count = mSettings->get<size_t>("Monsters", "count");

        for (size_t i = 0; i < count; i++)
        {
            exe.FAfseek(monsterOffset + 128 * i, SEEK_SET);

            Monster tmp(exe, codeOffset);

            if (mMonsters.find(tmp.monsterName) != mMonsters.end())
            {
                size_t j;
                for (j = 1; mMonsters.find(tmp.monsterName + "_" + std::to_string(j)) != mMonsters.end(); j++)
                    ;

                mMonsters[tmp.monsterName + "_" + std::to_string(j)] = tmp;
            }
            else
            {
                mMonsters[tmp.monsterName] = tmp;
            }
        }
    }

    void DiabloExe::loadTownerAnimation(FAIO::FAFileObject& exe)
    {
        auto offset = mSettings->get<int32_t>("TownerAnimation", "offset");
        auto size = mSettings->get<int32_t>("TownerAnimation", "size");
        auto count = mSettings->get<int32_t>("TownerAnimation", "count");
        exe.FAfseek(offset, SEEK_SET);
        mTownerAnimation.resize(count);
        for (int32_t i = 0; i < count; ++i)
        {
            mTownerAnimation[i].reserve(size);
            bool pastTheEnd = false;
            for (int32_t j = 0; j < size; ++j)
            {
                auto r = exe.read8();
                if (r == 255u)
                    pastTheEnd = true;
                if (!pastTheEnd)
                    mTownerAnimation[i].push_back(static_cast<int32_t>(r - 1));
            }
        }
    }

    void DiabloExe::loadNpcs(FAIO::FAFileObject& exe)
    {
        Settings::Container sections = mSettings->getSections();

        for (Settings::Container::const_iterator it = sections.begin(); it != sections.end(); ++it)
        {
            std::string name = *it;
            std::string section = name;

            if (Misc::StringUtils::startsWith(name, "NPC"))
            {
                auto& curNpc = mNpcs[name.substr(3, name.size() - 3)];
                curNpc = Npc(exe,
                             name,
                             mSettings->get<size_t>(section, "name"),
                             mSettings->get<size_t>(section, "cel"),
                             mSettings->get<size_t>(section, "x"),
                             mSettings->get<size_t>(section, "y"),
                             mSettings->get<size_t>(section, "rotation", 0));

                auto animId = mSettings->get<int32_t>(section, "animationId", -1);
                if (animId >= 0)
                    curNpc.animationSequenceId = animId;
                for (auto property : mSettings->getPropertiesInSection(section))
                {
                    std::string talkPrefix = "talk#";
                    std::string gossipPrefix = "gossip#";
                    std::string beforeDungeonPrefix = "beforeDungeon";
                    std::string questPrefix = "quest#";
                    if (Misc::StringUtils::startsWith(property, talkPrefix))
                    {
                        auto addr = mSettings->get<size_t>(section, property);
                        curNpc.menuTalkData[property.substr(talkPrefix.length())] = exe.readCString(addr);
                    }

                    else if (Misc::StringUtils::startsWith(property, gossipPrefix))
                    {
                        auto addr = mSettings->get<size_t>(section, property);
                        TalkData gossipData = {exe.readCString(addr), ""};
                        gossipData.text.pop_back();
                        curNpc.gossipData[property.substr(gossipPrefix.length())] = gossipData;
                    }

                    else if (property == beforeDungeonPrefix)
                    {
                        auto addr = mSettings->get<size_t>(section, property);
                        curNpc.beforeDungeonTalkData = {exe.readCString(addr), ""};
                        curNpc.beforeDungeonTalkData.text.pop_back();
                    }

                    /*else if (Misc::StringUtils::startsWith(property, questPrefix))
                    {
                        std::string quest = property.substr(questPrefix.size());
                        auto temp = Misc::StringUtils::split(quest, '#');
                        std::string questName = temp[0];
                        std::string questPhase = temp[1];
                        auto addr = mSettings->get<size_t>(section, property);
                        TalkData data = {exe.readCString(addr), ""};

                        if (questPhase == "info")
                            curNpc.questTalkData[questName].info = data;

                        else if (questPhase == "activation")
                            curNpc.questTalkData[questName].activation = data;

                        else if (questPhase == "return")
                            curNpc.questTalkData[questName].returned = {data};

                        else if (questPhase == "completion")
                            curNpc.questTalkData[questName].completion = data;

                        else
                            curNpc.questTalkData[questName].returned.push_back(data);
                    }*/
                }
            }
        }
    }

    void DiabloExe::loadBaseItems(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        size_t itemOffset = mSettings->get<size_t>("BaseItems", "itemOffset");
        size_t count = mSettings->get<size_t>("BaseItems", "count");
        size_t itemGraphicsIdToDropGraphicsIdOffset = mSettings->get<size_t>("ItemDropGraphics", "itemGraphicsIdToDropGraphicsId");
        std::vector<size_t> itemGraphicsIdToDropGraphicsId(172);

        exe.FAfseek(itemGraphicsIdToDropGraphicsIdOffset, SEEK_SET);
        for (auto& el : itemGraphicsIdToDropGraphicsId)
            el = exe.read8();

        std::array<std::array<int32_t, 2>, 180> objCursFrameSizes{{}};
        auto objCursFrameToWidthOffset = mSettings->get<size_t>("BaseItems", "objCursFrameToWidth");
        exe.FAfseek(objCursFrameToWidthOffset, SEEK_SET);
        for (auto& el : objCursFrameSizes)
            el[0] = exe.read32();
        auto objCursFrameToHeightOffset = mSettings->get<size_t>("BaseItems", "objCursFrameToHeight");
        exe.FAfseek(objCursFrameToHeightOffset, SEEK_SET);
        for (auto& el : objCursFrameSizes)
            el[1] = exe.read32();
        constexpr auto invCellSize = 28;
        for (size_t i = 0; i < count; i++)
        {
            exe.FAfseek(itemOffset + 76 * i, SEEK_SET);
            BaseItem tmp(exe, codeOffset);
            tmp.id = i;
            auto dropGraphicsId = itemGraphicsIdToDropGraphicsId[tmp.invGraphicsId];
            tmp.dropItemGraphicsPath = "items/" + mItemDropGraphicsFilename[dropGraphicsId] + ".cel";
            tmp.dropItemSoundPath = mSoundFilename[mItemGraphicsIdToDropSfxId[dropGraphicsId]];
            tmp.invPlaceItemSoundPath = mSoundFilename[mItemGraphicsIdToInvPlaceSfxId[dropGraphicsId]];
            auto& s = objCursFrameSizes[tmp.invGraphicsId + 11];
            if (i == 0)
            {
                tmp.invSizeX = 1;
                tmp.invSizeY = 1;
            }
            else
            {
                tmp.invSizeX = s[0] / invCellSize;
                tmp.invSizeY = s[1] / invCellSize;
            }
            mBaseItems.push_back(tmp);
        }
    }
    void DiabloExe::loadUniqueItems(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        size_t itemOffset = mSettings->get<size_t>("UniqueItems", "uniqueItemOffset");
        size_t count = mSettings->get<size_t>("UniqueItems", "count");

        for (size_t i = 0; i < count; i++)
        {
            exe.FAfseek(itemOffset + 84 * i, SEEK_SET);
            UniqueItem tmp(exe, codeOffset);
            if (tmp.mUniqueBaseItemId == 0 || tmp.mUniqueBaseItemId > 68)
                continue;

            if (Misc::StringUtils::containsNonPrint(tmp.mName))
                continue;

            mUniqueItems.push_back(tmp);
        }
    }

    void DiabloExe::loadAffixes(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        size_t affixOffset = mSettings->get<size_t>("Affix", "affixOffset");
        size_t count = mSettings->get<size_t>("Affix", "count");

        for (size_t i = 0; i < count; i++)
        {
            exe.FAfseek(affixOffset + 48 * i, SEEK_SET);
            Affix tmp(exe, codeOffset);
            if (Misc::StringUtils::containsNonPrint(tmp.mName))
                continue;
            if (tmp.mName.empty())
                continue;
            mAffixes.push_back(tmp);
        }
    }

    void DiabloExe::loadCharacterStats(FAIO::FAFileObject& exe)
    {
        size_t startingStatsOffset = mSettings->get<size_t>("CharacterStats", "startingStatsOffset");
        size_t maxStatsOffset = mSettings->get<size_t>("CharacterStats", "maxStatsOffset");
        size_t blockingBonusOffset = mSettings->get<size_t>("CharacterStats", "blockingBonusOffset");
        size_t framesetOffset = mSettings->get<size_t>("CharacterStats", "framesetOffset");
        size_t expPerLevelOffset = mSettings->get<size_t>("CharacterStats", "expPerLevelOffset");
        size_t levelCount = mSettings->get<size_t>("CharacterStats", "maxLevel");
        CharacterStats meleeCharacter, rangerCharacter, mageCharacter;

        exe.FAfseek(framesetOffset, SEEK_SET);

        meleeCharacter.mIdleInDungeonFrameset = exe.read8();
        rangerCharacter.mIdleInDungeonFrameset = exe.read8();
        mageCharacter.mIdleInDungeonFrameset = exe.read8();

        meleeCharacter.mAttackFrameset = exe.read8();
        rangerCharacter.mAttackFrameset = exe.read8();
        mageCharacter.mAttackFrameset = exe.read8();

        meleeCharacter.mWalkInDungeonFrameset = exe.read8();
        rangerCharacter.mWalkInDungeonFrameset = exe.read8();
        mageCharacter.mWalkInDungeonFrameset = exe.read8();

        meleeCharacter.mBlockingSpeed = exe.read8();
        rangerCharacter.mBlockingSpeed = exe.read8();
        mageCharacter.mBlockingSpeed = exe.read8();

        meleeCharacter.mDeathFrameset = exe.read8();
        rangerCharacter.mDeathFrameset = exe.read8();
        mageCharacter.mDeathFrameset = exe.read8();

        meleeCharacter.mMagicCastFrameset = exe.read8();
        rangerCharacter.mMagicCastFrameset = exe.read8();
        mageCharacter.mMagicCastFrameset = exe.read8();

        meleeCharacter.mHitRecoverySpeed = exe.read8();
        rangerCharacter.mHitRecoverySpeed = exe.read8();
        mageCharacter.mHitRecoverySpeed = exe.read8();

        meleeCharacter.mIdleInTownFrameset = exe.read8();
        rangerCharacter.mIdleInTownFrameset = exe.read8();
        mageCharacter.mIdleInTownFrameset = exe.read8();

        meleeCharacter.mWalkInTownFrameset = exe.read8();
        rangerCharacter.mWalkInTownFrameset = exe.read8();
        mageCharacter.mWalkInTownFrameset = exe.read8();

        meleeCharacter.mSingleHandedAttackSpeed = exe.read8();
        rangerCharacter.mSingleHandedAttackSpeed = exe.read8();
        mageCharacter.mSingleHandedAttackSpeed = exe.read8();

        meleeCharacter.mSpellCastSpeed = exe.read8();
        rangerCharacter.mSpellCastSpeed = exe.read8();
        mageCharacter.mSpellCastSpeed = exe.read8();

        exe.FAfseek(startingStatsOffset, SEEK_SET);

        meleeCharacter.mStrength = swapEndian(exe.read32());
        rangerCharacter.mStrength = swapEndian(exe.read32());
        mageCharacter.mStrength = swapEndian(exe.read32());

        meleeCharacter.mMagic = swapEndian(exe.read32());
        rangerCharacter.mMagic = swapEndian(exe.read32());
        mageCharacter.mMagic = swapEndian(exe.read32());

        meleeCharacter.mDexterity = swapEndian(exe.read32());
        rangerCharacter.mDexterity = swapEndian(exe.read32());
        mageCharacter.mDexterity = swapEndian(exe.read32());

        meleeCharacter.mVitality = swapEndian(exe.read32());
        rangerCharacter.mVitality = swapEndian(exe.read32());
        mageCharacter.mVitality = swapEndian(exe.read32());

        exe.FAfseek(blockingBonusOffset, SEEK_SET);

        meleeCharacter.mBlockingBonus = swapEndian(exe.read32());
        rangerCharacter.mBlockingBonus = swapEndian(exe.read32());
        mageCharacter.mBlockingBonus = swapEndian(exe.read32());

        exe.FAfseek(maxStatsOffset, SEEK_SET);

        meleeCharacter.mMaxStrength = exe.read32();
        rangerCharacter.mMaxStrength = exe.read32();
        mageCharacter.mMaxStrength = exe.read32();

        meleeCharacter.mMaxMagic = exe.read32();
        rangerCharacter.mMaxMagic = exe.read32();
        rangerCharacter.mMaxDexterity = exe.read32();

        meleeCharacter.mMaxDexterity = exe.read32();
        mageCharacter.mMaxMagic = exe.read32();
        mageCharacter.mMaxDexterity = exe.read32();

        meleeCharacter.mMaxVitality = exe.read32();
        rangerCharacter.mMaxVitality = exe.read32();
        mageCharacter.mMaxVitality = exe.read32();
        exe.FAfseek(expPerLevelOffset, SEEK_SET);

        for (uint32_t i = 0; i < levelCount; i++)
        {
            uint32_t readLevelData = exe.read32();
            meleeCharacter.mNextLevelExp.push_back(readLevelData);
            rangerCharacter.mNextLevelExp.push_back(readLevelData);
            mageCharacter.mNextLevelExp.push_back(readLevelData);
        }

        mCharacters["Warrior"] = meleeCharacter;
        mCharacters["Rogue"] = rangerCharacter;
        mCharacters["Sorceror"] = mageCharacter;
    }

    void DiabloExe::loadMissileGraphicsTable(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        const uint64_t offset = mSettings->get<uint64_t>("Missiles", "missileGraphicsOffset");
        const int len = mSettings->get<uint64_t>("Missiles", "missileGraphicsTableLen");
        const int rowSize = mSettings->get<uint64_t>("Missiles", "missileGraphicsRowSize");
        for (int i = 0; i < len; i++)
        {
            exe.FAfseek(offset + i * rowSize, SEEK_SET);
            auto missileGrapicsId = exe.read8();
            auto& missileGraphics = mMissileGraphicsTable[missileGrapicsId];
            missileGraphics.mNumAnimationFiles = exe.read8();
            exe.read16(); // Padding
            auto filenameOffset = exe.read32();
            missileGraphics.mFlags = exe.read32();
            for (int j = 0; j < 16; j++)
                exe.read32(); // NULL pointer
            for (auto& animationDelay : missileGraphics.mAnimationDelays)
                animationDelay = exe.read8();
            auto filename = exe.readCStringFromWin32Binary(filenameOffset, codeOffset);
            missileGraphics.mFilename = Misc::StringUtils::toLower(filename);
        }
    }

    void DiabloExe::loadMissileDataTable(FAIO::FAFileObject& exe)
    {
        const uint64_t offset = mSettings->get<uint64_t>("Missiles", "missileDataOffset");
        const int len = mSettings->get<uint64_t>("Missiles", "missileDataTableLen");
        const int rowSize = mSettings->get<uint64_t>("Missiles", "missileDataRowSize");
        for (int i = 0; i < len; i++)
        {
            exe.FAfseek(offset + i * rowSize, SEEK_SET);
            auto missileId = exe.read8();
            auto& missileData = mMissileDataTable[missileId];
            exe.read8();  // Padding
            exe.read16(); // Padding
            exe.read32(); // Function pointer
            exe.read32(); // Function pointer
            missileData.mDraw = exe.read32();
            missileData.mType = exe.read8();
            missileData.mResist = exe.read8();
            missileData.mMissileGraphicsId = exe.read8();
            exe.read8(); // Padding
            int32_t soundEffectId = exe.read32();
            missileData.mSoundEffect = soundEffectId == -1 ? "" : mSoundFilename[soundEffectId];
            int32_t impactSoundEffectId = exe.read32();
            missileData.mImpactSoundEffect = impactSoundEffectId == -1 ? "" : mSoundFilename[impactSoundEffectId];
        }
    }

    void DiabloExe::loadSpellsTable(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        const uint64_t offset = mSettings->get<uint64_t>("Spells", "spellsOffset");
        const int len = mSettings->get<uint64_t>("Spells", "spellsTableLen");
        const int rowSize = mSettings->get<uint64_t>("Spells", "spellsRowSize");
        for (int i = 0; i < len; i++)
        {
            exe.FAfseek(offset + i * rowSize, SEEK_SET);
            auto spellId = exe.read8();
            auto& spellData = mSpellsDataTable[spellId];
            spellData.mManaCost = exe.read8();
            int8_t type = exe.read8();
            switch (type)
            {
                case 0:
                    spellData.mType = SpellData::SpellType::fire;
                    break;
                case 1:
                    spellData.mType = SpellData::SpellType::lightning;
                    break;
                case 2:
                default:
                    spellData.mType = SpellData::SpellType::magic;
                    break;
            }
            exe.read8(); // Padding
            auto nameOffset = exe.read32();
            auto skillTextOffset = exe.read32();
            spellData.mBookLvl = exe.read32();
            spellData.mStaffLvl = exe.read32();
            spellData.mTargeted = (bool)exe.read32();
            spellData.mTownSpell = (bool)exe.read32();
            spellData.mMinMagic = exe.read32();
            int8_t soundEffectId = exe.read8();
            spellData.mSoundEffect = mSoundFilename[soundEffectId];
            for (auto& missile : spellData.mMissiles)
                missile = exe.read8();
            spellData.mManaAdj = exe.read8();
            spellData.mMinMana = exe.read8();
            exe.read16(); // Padding
            spellData.mStaffMin = exe.read32();
            spellData.mStaffMax = exe.read32();
            spellData.mBookCost = exe.read32();
            spellData.mStaffCost = exe.read32();
            spellData.mNameText = exe.readCStringFromWin32Binary(nameOffset, codeOffset);
            spellData.mSkillText = exe.readCStringFromWin32Binary(skillTextOffset, codeOffset);
        }
    }

    const Monster& DiabloExe::getMonster(const std::string& name) const { return mMonsters.at(name); }

    const CharacterStats DiabloExe::getCharacterStat(std::string character) const { return mCharacters.at(character); }

    std::vector<const Monster*> DiabloExe::getMonstersInLevel(size_t levelNum) const
    {
        std::vector<const Monster*> retval;

        for (std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            if (levelNum >= it->second.minDunLevel && levelNum <= it->second.maxDunLevel && it->second.monsterName != "Wyrm" &&
                it->second.monsterName != "Cave Slug" && it->second.monsterName != "Devil Wyrm" &&
                it->second.monsterName != "Devourer") // Exception, these monster's CEL files don't exist
            {
                retval.push_back(&(it->second));
            }
        }

        return retval;
    }

    const Npc& DiabloExe::getNpc(const std::string& name) const { return mNpcs.find(name)->second; }

    std::vector<const Npc*> DiabloExe::getNpcs() const
    {
        std::vector<const Npc*> retval;

        for (std::map<std::string, Npc>::const_iterator it = mNpcs.begin(); it != mNpcs.end(); ++it)
            retval.push_back(&(it->second));

        return retval;
    }

    const std::vector<std::vector<int32_t>>& DiabloExe::getTownerAnimation() const { return mTownerAnimation; }

    std::string DiabloExe::dump() const
    {
        std::stringstream ss;

        ss << "Monsters: " << mMonsters.size() << std::endl;
        for (std::map<std::string, Monster>::const_iterator it = mMonsters.begin(); it != mMonsters.end(); ++it)
        {
            ss << it->second.dump();
        }

        ss << "Npcs: " << mNpcs.size() << std::endl;
        for (std::map<std::string, Npc>::const_iterator it = mNpcs.begin(); it != mNpcs.end(); ++it)
        {
            ss << it->first << std::endl << it->second.dump();
        }

        ss << "Character Stats: " << mCharacters.size() << std::endl
           << "Warrior" << std::endl
           << mCharacters.at("Warrior").dump() << "Rogue" << std::endl
           << mCharacters.at("Rogue").dump() << "Sorceror" << std::endl
           << mCharacters.at("Sorceror").dump();

        ss << "Base Items: " << mBaseItems.size() << std::endl;
        for (auto& baseItem : mBaseItems)
            ss << baseItem.dump();

        ss << "Unique Items: " << mUniqueItems.size() << std::endl;
        for (auto& uniqueItem : mUniqueItems)
            ss << uniqueItem.dump();

        ss << "Affixes: " << mAffixes.size() << std::endl;
        for (auto& affix : mAffixes)
            ss << affix.dump();

        return ss.str();
    }

    bool DiabloExe::isLoaded() const { return !mMonsters.empty() && !mNpcs.empty() && !mBaseItems.empty() && !mAffixes.empty(); }
}
