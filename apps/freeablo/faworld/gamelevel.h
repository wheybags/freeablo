#ifndef FAWORLD_LEVEL_H
#define FAWORLD_LEVEL_H

#include <level/level.h>

#include "netobject.h"
#include "actor.h"

namespace FARender
{
    class Renderer;
}

namespace FAWorld
{
    class Player;

    class GameLevel : public NetObject
    {
        public:
            GameLevel(Level::Level level, size_t levelIndex, std::vector<Actor*> actors);
            ~GameLevel();

            Level::MinPillar getTile(size_t x, size_t y);

            size_t width() const;
            size_t height() const;

            const std::pair<size_t,size_t>& upStairsPos() const;
            const std::pair<size_t,size_t>& downStairsPos() const;

            void activate(size_t x, size_t y);

            size_t getNextLevel();
            size_t getPreviousLevel();

            virtual void startWriting();
            virtual size_t getWriteSize();
            virtual bool writeTo(ENetPacket* packet, size_t& position);
            virtual bool readFrom(ENetPacket* packet, size_t& position);

            void update(bool noclip, size_t tick);

            void actorMapInsert(Actor* actor);
            void actorMapRemove(Actor* actor);
            void actorMapClear();
            void actorMapRefresh();

            Actor* getActorAt(size_t x, size_t y);

            void addActor(Actor* actor);

            void fillRenderState(FARender::RenderState* state);

            void removeActor(Actor* actor);

            size_t getLevelIndex()
            {
                return mLevelIndex;
            }

            static GameLevel* fromPacket(ENetPacket* packet, size_t& position);

        private:
            GameLevel() {}

            Level::Level mLevel;
            size_t mLevelIndex;

            std::vector<Actor*> mActors;
            std::map<std::pair<size_t, size_t>, Actor*> mActorMap2D;    ///< Map of points to actors.
                                                                        ///< Where an actor straddles two squares, they shall be placed in both.

            std::string mDataSavingTmp;

            friend class FARender::Renderer;
    };
}

#endif
