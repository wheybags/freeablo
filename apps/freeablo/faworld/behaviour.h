#pragma once
#include "world.h"
#include <misc/misc.h>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{

    class Actor;
    class Player;

    class Behaviour
    {
    public:
        Behaviour(Actor* actor) { mActor = actor; }
        Behaviour() = default;

        virtual const std::string& getTypeId() = 0;
        virtual void save(FASaveGame::GameSaver& saver) const = 0;
        virtual void update() = 0;

        virtual ~Behaviour() {}

        virtual void reAttach(Actor* actor) { mActor = actor; } ///< only for use immediately after being loaded from a save game
    protected:
        Actor* mActor = nullptr;
    };

    // Does nothing
    class NullBehaviour : public Behaviour
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        NullBehaviour(FAWorld::Actor* actor) : Behaviour(actor) {}
        NullBehaviour() = default;

        virtual void save(FASaveGame::GameSaver&) const override {}
        virtual void update() override {}

        virtual ~NullBehaviour() {}
    };

    class BasicMonsterBehaviour : public Behaviour
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        BasicMonsterBehaviour(FAWorld::Actor* monster) : Behaviour(monster) {}
        BasicMonsterBehaviour(FASaveGame::GameLoader& loader);

        virtual void save(FASaveGame::GameSaver& saver) const override;
        virtual void update() override;

        virtual ~BasicMonsterBehaviour() {}

    private:
        Tick mTicksSinceLastAction = 0;
    };
}
