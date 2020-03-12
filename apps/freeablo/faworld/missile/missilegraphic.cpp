#include "missilegraphic.h"
#include "diabloexe/diabloexe.h"
#include "engine/enginemain.h"
#include "engine/threadmanager.h"
#include "fasavegame/gameloader.h"
#include "faworld/actor.h"

namespace FAWorld::Missile
{
    MissileGraphic::MissileGraphic(
        std::string initialGraphicPath, std::string mainGraphicPath, std::optional<int32_t> singleFrame, Position position, GameLevel* level)
        : mCurPos(position), mMainGraphicPath(mainGraphicPath), mSingleFrame(singleFrame), mLevel(level)
    {
        level->mMissileGraphics.insert(this);
        playAnimation(initialGraphicPath, FARender::AnimationPlayer::AnimationType::Once);
    }

    MissileGraphic::MissileGraphic(FASaveGame::GameLoader& loader)
    {
        mCurPos = Position(loader);
        mMainGraphicPath = loader.load<std::string>();

        mSingleFrame = loader.load<int32_t>();
        if (mSingleFrame == -1)
            mSingleFrame = std::nullopt;

        mAnimationPlayer = FARender::AnimationPlayer(loader);
        auto levelIndex = loader.load<int32_t>();
        auto world = loader.currentlyLoadingWorld;
        loader.addFunctionToRunAtEnd([this, world, levelIndex]() {
            mLevel = world->getLevel(levelIndex);
            mLevel->mMissileGraphics.insert(this);
        });
        mTicksSinceStarted = loader.load<Tick>();
        mComplete = loader.load<bool>();
    }

    MissileGraphic::~MissileGraphic() { mLevel->mMissileGraphics.erase(this); }

    void MissileGraphic::save(FASaveGame::GameSaver& saver) const
    {
        mCurPos.save(saver);
        saver.save(mMainGraphicPath);
        saver.save(static_cast<int32_t>(mSingleFrame == std::nullopt ? -1 : *mSingleFrame));
        mAnimationPlayer.save(saver);
        saver.save(mLevel->getLevelIndex());
        saver.save(mTicksSinceStarted);
        saver.save(mComplete);
    }

    void MissileGraphic::update()
    {
        mTicksSinceStarted++;

        if (mComplete)
            return;

        if (!mAnimationPlayer.isPlaying())
            playAnimation(mMainGraphicPath, FARender::AnimationPlayer::AnimationType::Looped);

        mAnimationPlayer.update();
    }

    std::pair<FARender::FASpriteGroup*, int32_t> MissileGraphic::getCurrentFrame()
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

    void MissileGraphic::playAnimation(std::string path, FARender::AnimationPlayer::AnimationType animationType)
    {
        if (!path.empty())
        {
            auto spriteGroup = FARender::Renderer::get()->loadImage(path);
            mAnimationPlayer.playAnimation(spriteGroup, World::getTicksInPeriod("0.06"), animationType);
        }
    }
}
