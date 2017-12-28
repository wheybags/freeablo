
#pragma once

#include <misc/maxcurrentitem.h>
#include <stdint.h>

namespace DiabloExe
{
    class CharacterStats;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class ActorStats
    {
    public:
        ActorStats() : mHp(100), mMana(100) {}
        ActorStats(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void takeDamage(int32_t damage) { mHp.change(-damage); }
        int32_t getAttackDamage() { return mAttackDamage; }

        Misc::MaxCurrentItem<int32_t> mHp;
        Misc::MaxCurrentItem<int32_t> mMana;
        int32_t mAttackDamage = 3;
    };
}
