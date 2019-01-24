#pragma once

#include "farender/animationplayer.h"
#include "missileenums.h"
#include <boost/optional.hpp>
#include <misc/misc.h>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    namespace Missile
    {
        class MissileGraphic
        {
        public:
            virtual ~MissileGraphic() = default;

            MissileGraphic(std::string initialGraphicPath, std::string mainGraphicPath, boost::optional<int32_t> singleFrame, Position position);
            MissileGraphic(FASaveGame::GameLoader& loader);

            virtual void save(FASaveGame::GameSaver& saver);
            virtual void update();

            std::pair<FARender::FASpriteGroup*, int32_t> getCurrentFrame();
            void stop();
            bool isComplete() const { return mComplete; }
            Tick getTicksSinceStarted() const { return mTicksSinceStarted; }

            Position mCurPos;

        protected:
            void playAnimation(std::string path, FARender::AnimationPlayer::AnimationType animationType);

            std::string mMainGraphicPath;
            boost::optional<int32_t> mSingleFrame;
            FARender::AnimationPlayer mAnimationPlayer;
            Tick mTicksSinceStarted = 0;
            bool mComplete = false;
        };
    }
}
