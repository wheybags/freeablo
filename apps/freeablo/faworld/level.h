#ifndef FAWORLD_LEVEL_H
#define FAWORLD_LEVEL_H

#include <level/level.h>

namespace FARender
{
    class Renderer;
}

namespace FAWorld
{
    class GameLevel
    {
        public:
            GameLevel(Level::Level level);

            std::vector<Level::Monster>& getMonsters();

            Level::MinPillar getTile(size_t x, size_t y);

            size_t width() const;
            size_t height() const;

            const std::pair<size_t,size_t>& upStairsPos() const;
            const std::pair<size_t,size_t>& downStairsPos() const;

            void activate(size_t x, size_t y);

            size_t getNextLevel();
            size_t getPreviousLevel();


        private:
            Level::Level mLevel;

            friend class FARender::Renderer;
    };
}

#endif
