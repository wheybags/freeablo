#ifndef CHARACTERSTATS_H
#define CHARACTERSTATS_H
#include "actorstats.hpp"
namespace FAWorld
{

    class WarriorStats : ActorStats
    {
    private:
        void recalculateDerivedStats() final;

    };

    class RogueStats : ActorStats
    {
    private:
        void recalculateDerivedStats() final;

    };

    class SorcererStats : ActorStats
    {
    private:
        void recalculateDerivedStats() final;

    };
}
#endif // CHARACTERSTATS_H
