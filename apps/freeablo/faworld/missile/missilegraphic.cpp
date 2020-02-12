#include "missilegraphic.h"

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
        MissileGraphic::MissileGraphic(std::string initialGraphicPath, std::string mainGraphicPath, std::optional<int32_t> singleFrame, Position position)
            : mCurPos(position), mMainGraphicPath(mainGraphicPath), mSingleFrame(singleFrame)
        {
            playAnimation(initialGraphicPath, FARender::AnimationPlayer::AnimationType::Once);
        }

        MissileGraphic::MissileGraphic(FASaveGame::GameLoader& loader)
        {
            mCurPos = Position(loader);
            mMainGraphicPath = loader.load<std::string>();
            mSingleFrame = (mSingleFrame = loader.load<int32_t>()) == -1 ? std::nullopt : mSingleFrame;
            mAnimationPlayer = FARender::AnimationPlayer(loader);
            mTicksSinceStarted = loader.load<Tick>();
            mComplete = loader.load<bool>();
        }

        void MissileGraphic::save(FASaveGame::GameSaver& saver)
        {
            mCurPos.save(saver);
            saver.save(mMainGraphicPath);
            saver.save(static_cast<int32_t>(mSingleFrame == std::nullopt ? -1 : *mSingleFrame));
            mAnimationPlayer.save(saver);
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

        void MissileGraphic::playAnimation(std::string path, FARender::AnimationPlayer::AnimationType animationType)
        {
            if (!path.empty())
            {
                auto spriteGroup = FARender::Renderer::get()->loadImage(path);
                mAnimationPlayer.playAnimation(spriteGroup, World::getTicksInPeriod("0.06"), animationType);
            }
        }
    }
}
