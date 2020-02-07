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
        void update(FAWorld::Actor& actor);
        void teleport(GameLevel* level, Position pos);
        void stopAndPointInDirection(Misc::Direction direction);

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
