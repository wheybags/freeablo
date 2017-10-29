#ifndef DIABLOCHARACTERSTATS_H
#define DIABLOCHARACTERSTATS_H
#include "actorstats.h"

namespace FAWorld
{
    class Inventory;
    class Player;

    class CharacterStatsBase : public ActorStats
    {
        public:
            CharacterStatsBase(DiabloExe::CharacterStats stats, Player * player) : ActorStats(stats), mPlayer(player){}
            virtual void processEffects();

        protected:
            Player * mPlayer;

    };

    class MeleeStats : public CharacterStatsBase
    {
    public:
        MeleeStats(DiabloExe::CharacterStats stats, Player * player) : CharacterStatsBase(stats, player)
        {
            mHP = 2*mVitality + 2*mLevel+18;
            mCurrentHP = mHP;
            mMana = mMagic + mLevel -1;
            mCurrentMana = mMana;
        }
        void recalculateDerivedStats() final;
    private:

        friend class Inventory;

    };

    class RangerStats : public CharacterStatsBase
    {
        public:
            RangerStats(DiabloExe::CharacterStats stats, Player * player) : CharacterStatsBase(stats, player)
            {
                mHP = mVitality + 2*mLevel+23;
                mCurrentHP = mHP;
                mMana = mMagic + 2*mLevel +5;
                mCurrentMana = mMana;
            }

            void recalculateDerivedStats() final;
        private:

            friend class Inventory;

    };

    class MageStats : public CharacterStatsBase
    {
        public:

            MageStats(DiabloExe::CharacterStats stats, Player * player) : CharacterStatsBase(stats, player)
            {
                mHP = mVitality + 2*mLevel + 9;
                mCurrentHP = mHP;
                mMana = 2*mMagic + 2*mLevel -2;
                mCurrentMana = mMana;
            }
            void recalculateDerivedStats() final;
        private:
            friend class Inventory;

    };
}
#endif // CHARACTERSTATS_H
