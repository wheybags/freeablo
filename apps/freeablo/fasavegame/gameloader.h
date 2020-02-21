#pragma once
#include <functional>
#include <serial/loader.h>
#include <vector>

namespace FAWorld
{
    class GameLevel;
    class World;
}

namespace FASaveGame
{
    class GameLoader : public Serial::Loader
    {
    public:
        explicit GameLoader(Serial::ReadStreamInterface& stream) : Serial::Loader(stream) {}

        void addFunctionToRunAtEnd(std::function<void()> func) { mFunctionsToRunAtEnd.push_back(func); }
        void runFunctionsToRunAtEnd();

        FAWorld::GameLevel* currentlyLoadingLevel = nullptr;
        FAWorld::World* currentlyLoadingWorld = nullptr;

    private:
        std::vector<std::function<void()>> mFunctionsToRunAtEnd;
    };

    class GameSaver : public Serial::Saver
    {
    public:
        explicit GameSaver(Serial::WriteStreamInterface& stream) : Serial::Saver(stream) {}
    };
}
