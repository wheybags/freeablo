#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <misc/misc.h>

#include "world.h"

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
        virtual const std::string& getTypeId() = 0;
        virtual void save(FASaveGame::GameSaver& saver) = 0;
        virtual ~Behaviour() {}

        void attach(Actor* actor);

        virtual void update() = 0;

    protected:
        Actor* mActor = nullptr;
    };

    // Does nothing
    class NullBehaviour : public Behaviour
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        void save(FASaveGame::GameSaver&) {}
        ~NullBehaviour() {}
        void update() {}
    };

    class BasicMonsterBehaviour : public Behaviour
    {
    public:
        BasicMonsterBehaviour() = default;
        BasicMonsterBehaviour(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) override;

        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        ~BasicMonsterBehaviour() {}
        void update();

    private:
        Tick mTicksSinceLastAction;
    };
}

#endif
