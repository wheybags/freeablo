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
        MovementHandler() = default;
        explicit MovementHandler(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

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
        Tick mPathRateLimit = World::getTicksInPeriod(1);

    private:
        GameLevel* mLevel = nullptr;
        Position mCurrentPos;
        Misc::Point mDestination;

        int32_t mCurrentPathIndex = 0;
        Misc::Points mCurrentPath;
        Tick mLastRepathed = std::numeric_limits<Tick>::min();
        bool mAdjacent = false;
    };
}
