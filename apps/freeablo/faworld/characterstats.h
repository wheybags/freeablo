#ifndef DIABLOCHARACTERSTATS_H
#define DIABLOCHARACTERSTATS_H
#include "actorstats.h"

namespace DiabloExe
{
  class CharacterStats;
}

namespace FAWorld
{
    class Inventory;
    class Player;

    class CharacterStatsBase : public ActorStats
    {
        public:
        CharacterStatsBase(DiabloExe::CharacterStats stats, Player* player);
            virtual void processEffects();

        protected:
            Player * mPlayer;

    };

    class MeleeStats : public CharacterStatsBase
    {
    public:
        MeleeStats(DiabloExe::CharacterStats stats, Player* player);
        void recalculateDerivedStats() final;
    private:

        friend class Inventory;

    };

    class RangerStats : public CharacterStatsBase
    {
        public:
        RangerStats(DiabloExe::CharacterStats stats, Player* player);

            void recalculateDerivedStats() final;
        private:

            friend class Inventory;

    };

    class MageStats : public CharacterStatsBase
    {
        public:

        MageStats(DiabloExe::CharacterStats stats, Player* player);
            void recalculateDerivedStats() final;
        private:
            friend class Inventory;

    };
}
#endif // CHARACTERSTATS_H
