#include "actorstats.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"

namespace FAWorld
{
    constexpr int32_t ActorStats::MAXIMUM_EXPERIENCE_POINTS;

    ActorStats::ActorStats(const Actor& actor, FASaveGame::GameLoader& loader) : mHp(loader.load<int32_t>()), mMana(loader.load<int32_t>()), mActor(actor)
    {
        baseStats.load(loader);
        mLevel = loader.load<int32_t>();
        mExperience = loader.load<int32_t>();

        size_t size = size_t(loader.load<int32_t>());
        mLevelXpCounts.reserve(size);
        for (size_t i = 0; i < size; i++)
            mLevelXpCounts.push_back(loader.load<int32_t>());
    }

    void ActorStats::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("ActorStats", saver);

        saver.save(mHp.current);
        saver.save(mMana.current);
        baseStats.save(saver);
        saver.save(mLevel);
        saver.save(mExperience);

        saver.save(int32_t(mLevelXpCounts.size()));
        for (size_t i = 0; i < mLevelXpCounts.size(); i++)
            saver.save(mLevelXpCounts[i]);
    }

    int32_t ActorStats::experiencePointsToLevel(uint32_t experience) const
    {
        int32_t i;
        for (i = 0; i < int32_t(mLevelXpCounts.size()); i++)
        {
            if (experience < mLevelXpCounts.at(i))
                break;
        }

        return std::min(i + 1, int32_t(mLevelXpCounts.size()));
    }

    void ActorStats::recalculateStats()
    {
        if (mActor->needsToRecalculateStats() || !mHasBeenCalculated)
        {
            mActor->calculateStats(mCalculatedStats);
            mHp.setMax(mCalculatedStats.maxLife);
            mMana.setMax(mCalculatedStats.maxMana);

            mHasBeenCalculated = true;
        }
    }

    void BaseStats::save(FASaveGame::GameSaver& saver)
    {
        saver.save(strength);
        saver.save(magic);
        saver.save(dexterity);
        saver.save(vitality);
    }

    void BaseStats::load(FASaveGame::GameLoader& loader)
    {
        strength = loader.load<int32_t>();
        magic = loader.load<int32_t>();
        dexterity = loader.load<int32_t>();
        vitality = loader.load<int32_t>();
    }
}
