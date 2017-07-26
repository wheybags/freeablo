#pragma once

#include "position.h"
#include "world.h"

namespace FAWorld
{
    class GameLevel;

    class MovementHandler
    {
    public:
        std::pair<int32_t, int32_t> getDestination() const;
        void setDestination(std::pair<int32_t, int32_t> dest);

        bool moving();
        Position getCurrentPosition() const;
        void update();
        void teleport(GameLevel* level, Position pos);

    private:
        GameLevel* mLevel;
        Position mCurrentPos;
        std::pair<int32_t, int32_t> mDestination;

        int32_t mCurrentPathIndex = 0;
        std::vector<std::pair<int32_t, int32_t>> mCurrentPath;
        Tick mTickSinceLastPath = std::numeric_limits<Tick>::max();
    };
}