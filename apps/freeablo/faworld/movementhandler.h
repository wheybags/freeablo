
#pragma once

#include "gamelevel.h"
#include "position.h"
#include "world.h"

#include <boost/signals2/signal.hpp>

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
        void update(const FAWorld::Actor& actor);
        void teleport(GameLevel* level, Position pos);
        void stopAndPointInDirection(Misc::Direction direction);

        boost::signals2::signal<void(const Misc::Point)> positionReached;

    private:
        bool positionReachedSent = true;
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
