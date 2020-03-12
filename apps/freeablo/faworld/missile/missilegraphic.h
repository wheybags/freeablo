#pragma once
#include "farender/animationplayer.h"
#include "missileenums.h"
#include <misc/misc.h>
#include <optional>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld::Missile
{
    class MissileGraphic
    {
    public:
        virtual ~MissileGraphic();

        MissileGraphic(std::string initialGraphicPath, std::string mainGraphicPath, std::optional<int32_t> singleFrame, Position position, GameLevel* level);
        MissileGraphic(FASaveGame::GameLoader& loader);

        virtual void save(FASaveGame::GameSaver& saver) const;
        virtual void update();

        std::pair<FARender::FASpriteGroup*, int32_t> getCurrentFrame();
        void stop();
        bool isComplete() const { return mComplete; }
        Tick getTicksSinceStarted() const { return mTicksSinceStarted; }
        GameLevel* getLevel() { return mLevel; }
        void setLevel(GameLevel* level);

        Position mCurPos;

    protected:
        void playAnimation(std::string path, FARender::AnimationPlayer::AnimationType animationType);

        std::string mMainGraphicPath;
        std::optional<int32_t> mSingleFrame;
        GameLevel* mLevel;
        FARender::AnimationPlayer mAnimationPlayer;
        Tick mTicksSinceStarted = 0;
        bool mComplete = false;
    };
}
