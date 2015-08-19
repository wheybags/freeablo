#ifndef DIABLOCHARACTERSTATS_H
#define DIABLOCHARACTERSTATS_H
#include "actorstats.hpp"
namespace FAWorld
{
    class Inventory;

    class MeleeStats : public ActorStats
    {
    public:
        MeleeStats(DiabloExe::CharacterStats stats) : ActorStats(stats)
        {
            mHP = 2*mVitality + 2*mLevel+18;
            mCurrentHP = mHP;
            mMana = mMagic + mLevel -1;


        }
        void recalculateDerivedStats() final;
    private:
        friend class Inventory;

    };

    class RangerStats : public ActorStats
    {
        public:
            RangerStats(DiabloExe::CharacterStats stats) : ActorStats(stats){}


        private:
            void recalculateDerivedStats() final;
            friend class Inventory;

    };

    class MageStats : public ActorStats
    {
        public:
            MageStats(DiabloExe::CharacterStats stats) : ActorStats(stats){}
        private:

            void recalculateDerivedStats() final;
            friend class Inventory;

    };
}
#endif // CHARACTERSTATS_H
