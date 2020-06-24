#include "missile.h"
#include "diabloexe/diabloexe.h"
#include "engine/enginemain.h"
#include "engine/threadmanager.h"
#include "fasavegame/gameloader.h"
#include "faworld/actor.h"

namespace FAWorld::Missile
{
    Missile::Missile(MissileId missileId, Actor& creator, Vec2Fix dest)
        : mCreator(&creator), mMissileId(missileId), mSrcPoint(creator.getPos().getFractionalPos()), mAttr(Attributes::fromId(missileId))
    {
        mAttr.mCreation(*this, dest, creator.getLevel());

        if (!missileData().mSoundEffect.empty())
            Engine::ThreadManager::get()->playSound(missileData().mSoundEffect);

        const LiveActorStats& stats = creator.mStats.getCalculatedStats();
        mToHitRanged = stats.toHitRanged;
        mToHitMinMaxCap = stats.toHitMinMaxCap;
        mRangedDamage = stats.rangedDamage;
        mRangedDamageBonusRange = stats.rangedDamageBonusRange;
    }

    Missile::Missile(FASaveGame::GameLoader& loader) : mMissileId(static_cast<MissileId>(loader.load<int32_t>())), mAttr(Attributes::fromId(mMissileId))
    {
        auto creatorId = loader.load<int32_t>();
        auto world = loader.currentlyLoadingWorld;
        loader.addFunctionToRunAtEnd([this, world, creatorId]() { mCreator = world->getActorById(creatorId); });

        mSrcPoint = Vec2Fix(loader);
        mComplete = loader.load<bool>();

        auto graphicsSize = loader.load<uint32_t>();
        mGraphics.reserve(graphicsSize);
        for (uint32_t i = 0; i < graphicsSize; i++)
            mGraphics.push_back(std::make_unique<MissileGraphic>(loader));

        mToHitRanged.load(loader);
        mToHitMinMaxCap = IntRange(loader);
        mRangedDamage = loader.load<int32_t>();
        mRangedDamageBonusRange = IntRange(loader);
    }

    void Missile::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("Missile", saver);

        saver.save(static_cast<int32_t>(mMissileId));
        saver.save(mCreator->getId());

        mSrcPoint.save(saver);
        saver.save(mComplete);

        saver.save(static_cast<uint32_t>(mGraphics.size()));
        for (auto& graphic : mGraphics)
            graphic->save(saver);

        mToHitRanged.save(saver);
        mToHitMinMaxCap.save(saver);
        saver.save(mRangedDamage);
        mRangedDamageBonusRange.save(saver);
    }

    const DiabloExe::MissileData& Missile::missileData() const
    {
        const auto& missileDataTable = Engine::EngineMain::get()->exe().getMissileDataTable();
        return missileDataTable.at((size_t)mMissileId);
    }

    const FARender::SpriteLoader::SpriteDefinition& Missile::getGraphic(int32_t i) const
    {
        FARender::SpriteLoader& spriteLoader = FARender::Renderer::get()->mSpriteLoader;

        const std::vector<FARender::SpriteLoader::SpriteDefinition>& directions = spriteLoader.mMissileAnimations[missileData().mMissileGraphicsId];
        release_assert(i >= 0 && i < int32_t(directions.size()));

        return directions[i];
    }

    void Missile::playImpactSound()
    {
        if (!missileData().mImpactSoundEffect.empty())
            Engine::ThreadManager::get()->playSound(missileData().mImpactSoundEffect);
    }

    void Missile::update()
    {
        for (auto& graphic : mGraphics)
        {
            if (graphic->isComplete())
                continue;

            graphic->update();

            mAttr.mMovement(*this, *graphic);

            auto curPoint = graphic->mCurPos.current();

            // Check if actor is hit.
            auto actor = graphic->getLevel()->getActorAt(curPoint);
            if (actor)
                mAttr.mActorEngagement(*this, *graphic, *actor);

            // Stop when walls are hit.
            if (!actor && !graphic->getLevel()->isPassable(curPoint, mCreator))
            {
                playImpactSound();
                graphic->stop();
            }

            // Stop after "time to live" has expired.
            if (graphic->getTicksSinceStarted() > mAttr.mTimeToLive)
                graphic->stop();
        }
        // Set complete flag when all graphics are finished.
        mComplete = std::all_of(mGraphics.begin(), mGraphics.end(), [](const std::unique_ptr<MissileGraphic>& graphic) { return graphic->isComplete(); });
    }
}
