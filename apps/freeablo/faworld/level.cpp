#include "level.h"

namespace FAWorld
{
    GameLevel::GameLevel(Level::Level level) : mLevel(level)
    { }

    std::vector<Level::Monster>& GameLevel::getMonsters()
    {
        return mLevel.getMonsters();
    }

    Level::MinPillar GameLevel::getTile(size_t x, size_t y)
    {
        return mLevel[x][y];
    }

    size_t GameLevel::width() const
    {
        return mLevel.width();
    }

    size_t GameLevel::height() const
    {
        return mLevel.height();
    }

    const std::pair<size_t,size_t>& GameLevel::upStairsPos() const
    {
        return mLevel.upStairsPos();
    }

    const std::pair<size_t,size_t>& GameLevel::downStairsPos() const
    {
        return mLevel.downStairsPos();
    }

    void GameLevel::activate(size_t x, size_t y)
    {
        mLevel.activate(x, y);
    }

    size_t GameLevel::getNextLevel()
    {
        return mLevel.getNextLevel();
    }

    size_t GameLevel::getPreviousLevel()
    {
        return mLevel.getPreviousLevel();
    }
}
