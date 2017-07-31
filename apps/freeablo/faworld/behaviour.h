#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <assert.h>

#include <misc/misc.h>

#include "world.h"
#include "netobject.h"

namespace FAWorld
{

    class Actor;
    class Player;

    class Behaviour : public NetObject
    {
    public:
        virtual ~Behaviour() {};

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
        ~NullBehaviour() {};
        void update() {
        };

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
        ~BasicMonsterBehaviour() {};
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
