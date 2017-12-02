#include "gameloader.h"

namespace FASaveGame
{
    void GameLoader::runFunctionsToRunAtEnd()
    {
        for (auto& func : mFunctionsToRunAtEnd)
            func();

        mFunctionsToRunAtEnd.clear();
    }
}
