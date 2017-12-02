#pragma once

#include "position.h"
#include "world.h"
#include "gamelevel.h"

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

        std::pair<int32_t, int32_t> getDestination() const;
        void setDestination(std::pair<int32_t, int32_t> dest, bool adjacent = false);

        bool moving();
        Position getCurrentPosition() const;
        GameLevel* getLevel();
        void update(int32_t actorId);
        void teleport(GameLevel* level, Position pos);

    private:
        GameLevel* mLevel = nullptr;
        Position mCurrentPos;
        std::pair<int32_t, int32_t> mDestination;

        int32_t mCurrentPathIndex = 0;
        std::vector<std::pair<int32_t, int32_t>> mCurrentPath;
        Tick mLastRepathed = std::numeric_limits<Tick>::min();
        Tick mPathRateLimit;
        bool mAdjacent = false;
    };
}
