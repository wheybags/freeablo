#include "missilegraphic.h"
#include "engine/enginemain.h"
#include "engine/threadmanager.h"
#include "faworld/actor.h"
#include "missile.h"
#include <engine/debugsettings.h>
#include <utility>

namespace FAWorld::Missile
{
    MissileGraphic::MissileGraphic(FARender::SpriteLoader::SpriteDefinition initialGraphic,
                                   FARender::SpriteLoader::SpriteDefinition mainGraphic,
                                   std::optional<int32_t> singleFrame,
                                   Position position,
                                   GameLevel* level)
        : mCurPos(position), mInitialGraphic(std::move(initialGraphic)), mMainGraphic(std::move(mainGraphic)), mSingleFrame(singleFrame), mLevel(level)
    {
        level->mMissileGraphics.insert(this);

        FARender::SpriteLoader& spriteLoader = FARender::Renderer::get()->mSpriteLoader;
        if (!mInitialGraphic.empty())
        {
            Render::SpriteGroup* sprite = spriteLoader.getSprite(mInitialGraphic);
            playAnimation(sprite, FARender::AnimationPlayer::AnimationType::Once);
        }
        else if (!mMainGraphic.empty())
        {
            Render::SpriteGroup* sprite = spriteLoader.getSprite(mMainGraphic);
            playAnimation(sprite, FARender::AnimationPlayer::AnimationType::Looped);
        }
    }

    MissileGraphic::MissileGraphic(FASaveGame::GameLoader& loader)
    {
        mCurPos = Position(loader);

        mSingleFrame = loader.load<int32_t>();
        if (mSingleFrame == -1)
            mSingleFrame = std::nullopt;

        mAnimationPlayer.load(loader);
        auto levelIndex = loader.load<int32_t>();
        auto world = loader.currentlyLoadingWorld;
        loader.addFunctionToRunAtEnd([this, world, levelIndex]() {
            mLevel = world->getLevel(levelIndex);
            mLevel->mMissileGraphics.insert(this);
        });
        mTicksSinceStarted = loader.load<Tick>();
        mComplete = loader.load<bool>();

        mInitialGraphic.load(loader);
        mMainGraphic.load(loader);

        // restore animation
        if (!mComplete)
        {
            if (!mInitialGraphic.empty())
                mAnimationPlayer.replaceAnimation(FARender::Renderer::get()->mSpriteLoader.getSprite(mInitialGraphic));
            else if (!mMainGraphic.empty())
                mAnimationPlayer.replaceAnimation(FARender::Renderer::get()->mSpriteLoader.getSprite(mMainGraphic));
        }
        else
        {
            mAnimationPlayer.stopAnimation();
        }

        mAnimationPlayer.animationRestoredAfterSave = true;
    }

    MissileGraphic::~MissileGraphic() { mLevel->mMissileGraphics.erase(this); }

    void MissileGraphic::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("MissileGraphic", saver);

        mCurPos.save(saver);

        saver.save(static_cast<int32_t>(mSingleFrame == std::nullopt ? -1 : *mSingleFrame));
        mAnimationPlayer.save(saver);
        saver.save(mLevel->getLevelIndex());
        saver.save(mTicksSinceStarted);
        saver.save(mComplete);

        mInitialGraphic.save(saver);
        mMainGraphic.save(saver);
    }

    void MissileGraphic::update()
    {
        if (DebugSettings::DebugMissiles)
        {
            Vec2Fix currentTileCentre = Vec2Fix(mCurPos.current()) + Vec2Fix(FixedPoint("0.5"), FixedPoint("0.5"));
            FARender::Renderer::get()->mTmpDebugRenderData.push_back(PointData{currentTileCentre, Render::Colors::green, 5});
            FARender::Renderer::get()->mTmpDebugRenderData.push_back(PointData{mCurPos.getFractionalPos(), Render::Colors::red, 1});
        }

        mTicksSinceStarted++;

        if (mComplete)
            return;

        mAnimationPlayer.update();

        if (!mAnimationPlayer.isPlaying())
        {
            mInitialGraphic.clear();
            playAnimation(FARender::Renderer::get()->mSpriteLoader.getSprite(mMainGraphic), FARender::AnimationPlayer::AnimationType::Looped);
        }
    }

    std::pair<Render::SpriteGroup*, int32_t> MissileGraphic::getCurrentFrame()
    {
        auto frame = mAnimationPlayer.getCurrentFrame();
        // Some animations just use a single offset frame.
        // e.g. arrows have a single frame for each of the 16 directions.
        if (mSingleFrame)
            frame.second = *mSingleFrame;
        return frame;
    }

    void MissileGraphic::stop()
    {
        mAnimationPlayer.stopAnimation();
        mComplete = true;
    }

    void MissileGraphic::setLevel(GameLevel* level)
    {
        mLevel->mMissileGraphics.erase(this);
        mLevel = level;
        mLevel->mMissileGraphics.insert(this);
    }

    void MissileGraphic::playAnimation(Render::SpriteGroup* spriteGroup, FARender::AnimationPlayer::AnimationType animationType)
    {
        debug_assert(spriteGroup);
        mAnimationPlayer.playAnimation(spriteGroup, World::getTicksInPeriod("0.06"), animationType);
    }
}
