#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <misc/misc.h>

namespace FAWorld
{

    class Actor;
    class Player;

    class Behaviour
    {
    public:
        Behaviour(Actor* actor): mActor(actor) {};
        virtual ~Behaviour() {};

        virtual void update(size_t ticksPassed) = 0;

    protected:
        Actor * mActor;

    };

    // Does nothing
    class NullBehaviour : public Behaviour
    {
    public:
        NullBehaviour(Actor* actor): Behaviour(actor) {};
        ~NullBehaviour() {};
        void update(size_t ticksPassed) {
            UNUSED_PARAM(ticksPassed);
        };
    };

    class BasicMonsterBehaviour : public Behaviour
    {
    public:
        BasicMonsterBehaviour(Actor* actor): Behaviour(actor) {};
        ~BasicMonsterBehaviour() {};
        void update(size_t ticksPassed);

    private:
        size_t mLastActionTick;
    };

}

#endif
