#include "playerstats.h"
#include "../fasavegame/gameloader.h"
#include "diabloexe/characterstats.h"

namespace FAWorld
{
    PlayerStats::PlayerStats(const DiabloExe::CharacterStats& charStats)
        : mStrength(charStats.mStrength), mDexterity(charStats.mDexterity), mMagic(charStats.mMagic), mVitality(charStats.mVitality),
          mNextLevelExp(charStats.mNextLevelExp)
    {
    }

    PlayerStats::PlayerStats(FASaveGame::GameLoader& loader)
    {
        mStrength = loader.load<int32_t>();
        mDexterity = loader.load<int32_t>();
        mMagic = loader.load<int32_t>();
        mVitality = loader.load<int32_t>();
        mExp = loader.load<uint32_t>();
        mNextLevelExp.resize(loader.load<uint32_t>());
        for (auto& x : mNextLevelExp)
            x = loader.load<uint32_t>();

        mLevel = expToLevel(mExp);
    }

    void PlayerStats::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("PlayerStats", saver);

        saver.save(mStrength);
        saver.save(mDexterity);
        saver.save(mMagic);
        saver.save(mVitality);
        saver.save(mExp);
        saver.save((uint32_t)mNextLevelExp.size());
        for (auto& x : mNextLevelExp)
            saver.save(x);
    }

    int32_t PlayerStats::expToLevel(uint32_t exp) const
    {
        int32_t i;
        for (i = 0; i < (int32_t)mNextLevelExp.size(); i++)
            if (exp < mNextLevelExp.at(i))
                break;

        return std::min(i + 1, (int32_t)mNextLevelExp.size());
    }

    uint32_t PlayerStats::nextLevelExp() const { return mNextLevelExp.at(mLevel - 1); }

    uint32_t PlayerStats::maxExp() const { return 2000000000; }
}
