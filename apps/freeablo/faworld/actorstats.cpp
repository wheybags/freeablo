#include "actorstats.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"

namespace FAWorld
{
    constexpr int32_t ActorStats::MAXIMUM_EXPERIENCE_POINTS;

    void ActorStats::initialise(const BaseStats& baseStats)
    {
        this->baseStats = baseStats;

        getHp().current = getHp().max;
        getMana().current = getMana().max;
    }

    ActorStats::ActorStats(const Actor& actor, FASaveGame::GameLoader& loader) : mActor(actor), mHp(loader.load<int32_t>()), mMana(loader.load<int32_t>())
    {
        baseStats.load(loader);
        mLevel = loader.load<int32_t>();
        mExperience = loader.load<int32_t>();

        size_t size = size_t(loader.load<int32_t>());
        mLevelXpCounts.reserve(size);
        for (size_t i = 0; i < size; i++)
            mLevelXpCounts.push_back(loader.load<uint32_t>());
    }

    void ActorStats::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("ActorStats", saver);

        saver.save(getHp().current);
        saver.save(getMana().current);
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

    const LiveActorStats& ActorStats::getCalculatedStats() const
    {
        recalculateStats();
        return mCalculatedStats;
    }

    void ActorStats::recalculateStats() const
    {
        mActor->calculateStats(mCalculatedStats, *this);

        if (mCalculatedStats.maxLife != -1)
            mHp.setMax(mCalculatedStats.maxLife);
        if (mCalculatedStats.maxMana != -1)
            mMana.setMax(mCalculatedStats.maxMana);
    }

    void BaseStats::save(FASaveGame::GameSaver& saver) const
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
