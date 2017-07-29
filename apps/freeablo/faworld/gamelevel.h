#ifndef FAWORLD_LEVEL_H
#define FAWORLD_LEVEL_H

#include <level/level.h>
#include <unordered_map>

#include <enet/enet.h> // TODO: remove

#include <misc/stdhashes.h>

namespace FARender
{
    class Renderer;
    class RenderState;
}

namespace FAWorld
{
    class Actor;


    class GameLevelImpl
    {
    public:
        virtual ~GameLevelImpl() {}
        virtual size_t width() const = 0;
        virtual size_t height() const = 0;
        virtual bool isPassable(int x, int y) const = 0;
    };

    class GameLevel :public GameLevelImpl
    {
    public:
        GameLevel(Level::Level level, size_t levelIndex);
        ~GameLevel();

        Level::MinPillar getTile(size_t x, size_t y);

        size_t width() const;
        size_t height() const;

        const std::pair<size_t, size_t>& upStairsPos() const;
        const std::pair<size_t, size_t>& downStairsPos() const;

        void activate(size_t x, size_t y);

        size_t getNextLevel();
        size_t getPreviousLevel();

        void update(bool noclip);

        void actorMapInsert(Actor* actor);
        void actorMapRemove(Actor* actor);
        void actorMapClear();
        void actorMapRefresh();
        virtual bool isPassable(int x, int y) const;

        Actor* getActorAt(int32_t x, int32_t y) const;

        void addActor(Actor* actor);

        void fillRenderState(FARender::RenderState* state);

        void removeActor(Actor* actor);

        size_t getLevelIndex()
        {
            return mLevelIndex;
        }

        std::string serialiseToString();
        static GameLevel* loadFromString(const std::string& data);

        Actor* getActorById(int32_t id);

        void getActors(std::vector<Actor*>& actors);

    private:
        GameLevel() {}

        Level::Level mLevel;
        size_t mLevelIndex;

        std::vector<Actor*> mActors;
        std::unordered_map<std::pair<int32_t, int32_t>, Actor*> mActorMap2D;    ///< Map of points to actors.
                                                                    ///< Where an actor straddles two squares, they shall be placed in both.
        friend class FARender::Renderer;
    };
}

#endif
