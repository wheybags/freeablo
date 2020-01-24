#include "actorstats.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"

namespace FAWorld
{
    ActorStats::ActorStats(const Actor& actor, FASaveGame::GameLoader& loader)
        : mHp(loader.load<int32_t>()), mMana(loader.load<int32_t>()), mAttackDamage(loader.load<int32_t>()), mActor(actor)
    {
    }

    void ActorStats::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("ActorStats", saver);

        saver.save(mHp.current);
        saver.save(mMana.current);
        saver.save(mAttackDamage);
    }

    void ActorStats::recalculateStats()
    {
        if (mActor.needsToRecalculateStats() || !mHasBeenCalculated)
        {
            mActor.calculateStats(mCalculatedStats);
            mHp.setMax(mCalculatedStats.maxLife);
            mMana.setMax(mCalculatedStats.maxMana);

            mHasBeenCalculated = true;
        }
    }
}
