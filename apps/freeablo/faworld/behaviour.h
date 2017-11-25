#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <assert.h>

#include <misc/misc.h>

#include "world.h"
#include "netobject.h"

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{

    class Actor;
    class Player;

    class Behaviour : public NetObject
    {
    public:
        virtual const std::string& getTypeId() = 0;
        virtual void save(FASaveGame::GameSaver& saver) = 0;
        virtual ~Behaviour() {}

        void attach(Actor* actor) 
        {
            assert(mActor == nullptr);
            mActor = actor;
        }

        virtual void update() = 0;

    protected:
        Actor * mActor = nullptr;
    };

    // Does nothing
    class NullBehaviour : public Behaviour
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        void save(FASaveGame::GameSaver&) {}
        ~NullBehaviour() {}
        void update() {}

    protected:
        template <class Stream>
        Serial::Error::Error faSerial(Stream& stream)
        {
            UNUSED_PARAM(stream);
            return Serial::Error::Success;
        }

        friend class Serial::WriteBitStream;
        friend class Serial::ReadBitStream;
    };

    class BasicMonsterBehaviour : public Behaviour
    {
        STATIC_HANDLE_NET_OBJECT_IN_CLASS()

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

    protected:
        template <class Stream>
        Serial::Error::Error faSerial(Stream& stream)
        {
            UNUSED_PARAM(stream);
            return Serial::Error::Success;
        }

        friend class Serial::WriteBitStream;
        friend class Serial::ReadBitStream;
    };

}

#endif
