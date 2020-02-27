#pragma once
#include "gamelevel.h"
#include "position.h"
#include "world.h"

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class GameLevel;

    class MovementHandler
    {
    public:
        MovementHandler(Tick pathRateLimit);

        MovementHandler(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        Misc::Point getDestination() const;
        void setDestination(Misc::Point dest, bool adjacent = false);

        bool moving();
        const Position& getCurrentPosition() const { return mCurrentPos; }
        GameLevel* getLevel();
        const GameLevel* getLevel() const;
        void update(Actor& actor);
        void teleport(GameLevel* level, Position pos);
        void stopMoving(FAWorld::Actor& actor, std::optional<Misc::Direction> pointInDirection = std::nullopt);

    private:
        FixedPoint updateInternal(Actor& actor, FixedPoint moveDistance);

    public:
        FixedPoint mSpeedTilesPerSecond;

    private:
        GameLevel* mLevel = nullptr;
        Position mCurrentPos;
        Misc::Point mDestination;

        int32_t mCurrentPathIndex = 0;
        Misc::Points mCurrentPath;
        Tick mLastRepathed = std::numeric_limits<Tick>::min();
        Tick mPathRateLimit;
        bool mAdjacent = false;
    };
}
