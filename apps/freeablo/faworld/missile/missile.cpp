#include "missile.h"
#include "diabloexe/diabloexe.h"
#include "engine/enginemain.h"
#include "engine/threadmanager.h"
#include "fasavegame/gameloader.h"
#include "faworld/actor.h"
#include <misc/vec2fix.h>

namespace FAWorld
{
    namespace Missile
    {
        Missile::Missile(MissileId missileId, Actor& creator, Misc::Point dest)
            : mCreator(&creator), mMissileId(missileId), mLevel(creator.getLevel()), mSrcPoint(creator.getPos().current())
        {
            MissileCreation::get(missileId)(*this, dest);

            if (!missileData().mSoundEffect.empty())
                Engine::ThreadManager::get()->playSound(missileData().mSoundEffect);
        }

        Missile::Missile(FASaveGame::GameLoader& loader)
        {
            auto creatorId = loader.load<int32_t>();
            auto levelIndex = loader.load<int32_t>();
            auto world = loader.currentlyLoadingWorld;
            loader.addFunctionToRunAtEnd([this, world, creatorId, levelIndex]() {
                mCreator = world->getActorById(creatorId);
                mLevel = world->getLevel(levelIndex);
            });

            mMissileId = static_cast<MissileId>(loader.load<int32_t>());
            mSrcPoint = Misc::Point(loader);
            mComplete = loader.load<bool>();

            auto graphicsSize = loader.load<uint32_t>();
            mGraphics.reserve(graphicsSize);
            for (uint32_t i = 0; i < graphicsSize; i++)
                mGraphics.push_back(std::make_unique<MissileGraphic>(loader));
        }

        void Missile::save(FASaveGame::GameSaver& saver)
        {
            Serial::ScopedCategorySaver cat("Missile", saver);

            saver.save(mCreator->getId());
            saver.save(mLevel->getLevelIndex());

            saver.save(static_cast<int32_t>(mMissileId));
            mSrcPoint.save(saver);
            saver.save(mComplete);

            saver.save(static_cast<uint32_t>(mGraphics.size()));
            for (auto& graphic : mGraphics)
                graphic->save(saver);
        }

        const DiabloExe::MissileData& Missile::missileData() const
        {
            const auto& missileDataTable = Engine::EngineMain::get()->exe().getMissileDataTable();
            return missileDataTable.at((size_t)mMissileId);
        }

        const DiabloExe::MissileGraphics& Missile::missileGraphics() const
        {
            const auto& missileGraphicsTable = Engine::EngineMain::get()->exe().getMissileGraphicsTable();
            return missileGraphicsTable.at((size_t)missileData().mMissileGraphicsId);
        }

        std::string Missile::getGraphicsPath(int32_t i) const
        {
            release_assert(i >= 0 && i < missileGraphics().mNumAnimationFiles);
            std::stringstream path;
            path << "missiles/" << missileGraphics().mFilename;
            if (missileGraphics().mNumAnimationFiles > 1)
                path << i + 1;
            path << ".cl2";
            return path.str();
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

                MissileMovement::get(mMissileId)(*this, *graphic);

                auto curPoint = graphic->mCurPos.current();

                // Check if actor is hit.
                auto actor = mLevel->getActorAt(curPoint);
                if (actor)
                    MissileActorEngagement::get(mMissileId)(*this, *graphic, *actor);

                // Stop when walls are hit.
                if (!actor && !mLevel->isPassable(curPoint, mCreator))
                {
                    playImpactSound();
                    graphic->stop();
                }

                // Stop after max range is exceeded.
                auto distance = (Vec2Fix(curPoint.x, curPoint.y) - Vec2Fix(mSrcPoint.x, mSrcPoint.y)).magnitude();
                if (distance > MissileMaxRange::get(mMissileId))
                    graphic->stop();

                // Stop after "time to live" has expired.
                if (graphic->getTicksSinceStarted() > MissileTimeToLive::get(mMissileId))
                    graphic->stop();
            }
            // Set complete flag when all graphics are finished.
            mComplete = std::all_of(mGraphics.begin(), mGraphics.end(), [](const std::unique_ptr<MissileGraphic>& graphic) { return graphic->isComplete(); });
        }
    }
}
