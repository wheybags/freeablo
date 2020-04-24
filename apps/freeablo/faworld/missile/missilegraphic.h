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

        MissileGraphic(FARender::SpriteLoader::SpriteDefinition initialGraphic,
                       FARender::SpriteLoader::SpriteDefinition mainGraphic,
                       std::optional<int32_t> singleFrame,
                       Position position,
                       GameLevel* level);
        explicit MissileGraphic(FASaveGame::GameLoader& loader);

        virtual void save(FASaveGame::GameSaver& saver) const;
        virtual void update();

        std::pair<Render::SpriteGroup*, int32_t> getCurrentFrame();
        void stop();
        bool isComplete() const { return mComplete; }
        Tick getTicksSinceStarted() const { return mTicksSinceStarted; }
        GameLevel* getLevel() { return mLevel; }
        void setLevel(GameLevel* level);

        Position mCurPos;

    protected:
        void playAnimation(Render::SpriteGroup* spriteGroup, FARender::AnimationPlayer::AnimationType animationType);

        FARender::SpriteLoader::SpriteDefinition mInitialGraphic;
        FARender::SpriteLoader::SpriteDefinition mMainGraphic;

        std::optional<int32_t> mSingleFrame;
        GameLevel* mLevel;
        FARender::AnimationPlayer mAnimationPlayer;
        Tick mTicksSinceStarted = 0;
        bool mComplete = false;
    };
}
