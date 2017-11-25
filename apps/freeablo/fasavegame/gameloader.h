#pragma once

#include <serial/loader.h>

namespace FASaveGame
{
    class GameLoader : public Serial::Loader
    {
    public:
        GameLoader(Serial::ReadBitStream& stream) : Serial::Loader(stream) {}
    };

    class GameSaver : public Serial::Saver
    {
    public:
        GameSaver(Serial::WriteBitStream& stream) : Serial::Saver(stream) {}
    };
}
