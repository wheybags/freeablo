#ifndef FAWORLD_LEVEL_H
#define FAWORLD_LEVEL_H

#include <level/level.h>

#include "netobject.h"

namespace FARender
{
    class Renderer;
}

namespace FAWorld
{
    class GameLevel : public NetObject
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

            virtual void startSaving();
            virtual size_t getWriteSize();
            virtual bool writeTo(ENetPacket* packet, size_t& position);
            virtual bool readFrom(ENetPacket* packet, size_t& position);

        private:
            Level::Level mLevel;

            std::string mDataSavingTmp;

            friend class FARender::Renderer;
    };
}

#endif
