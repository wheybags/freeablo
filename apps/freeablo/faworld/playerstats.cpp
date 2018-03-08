#include "playerstats.h"
#include "../fasavegame/gameloader.h"
#include "diabloexe/characterstats.h"

namespace FAWorld
{
    PlayerStats::PlayerStats(const DiabloExe::CharacterStats& charStats)
        : mStrength(charStats.mStrength), mDexterity(charStats.mDexterity), mMagic(charStats.mMagic), mVitality(charStats.mVitality)
    {
    }

    PlayerStats::PlayerStats(FASaveGame::GameLoader& loader)
    {
        mStrength = loader.load<int32_t>();
        mDexterity = loader.load<int32_t>();
        mMagic = loader.load<int32_t>();
        mVitality = loader.load<int32_t>();
    }

    void PlayerStats::save(FASaveGame::GameSaver& saver) const
    {
        saver.save(mStrength);
        saver.save(mDexterity);
        saver.save(mMagic);
        saver.save(mVitality);
    }
}
