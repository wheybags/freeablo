
#pragma once

#include <functional>
#include <serial/loader.h>
#include <vector>

namespace FASaveGame
{
    class GameLoader : public Serial::Loader
    {
    public:
        GameLoader(Serial::ReadStreamInterface& stream) : Serial::Loader(stream) {}

        void addFunctionToRunAtEnd(std::function<void()> func) { mFunctionsToRunAtEnd.push_back(func); }
        void runFunctionsToRunAtEnd();

    private:
        std::vector<std::function<void()>> mFunctionsToRunAtEnd;
    };

    class GameSaver : public Serial::Saver
    {
    public:
        GameSaver(Serial::WriteStreamInterface& stream) : Serial::Saver(stream) {}
    };
}
