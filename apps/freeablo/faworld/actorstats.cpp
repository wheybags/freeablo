#include "actorstats.h"
#include "../fasavegame/gameloader.h"

namespace FAWorld
{
    ActorStats::ActorStats(FASaveGame::GameLoader& loader) : mHp(loader), mMana(loader), mAttackDamage(loader.load<int32_t>()) {}

    void ActorStats::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("ActorStats", saver);

        mHp.save(saver);
        mMana.save(saver);
        saver.save(mAttackDamage);
    }
}
