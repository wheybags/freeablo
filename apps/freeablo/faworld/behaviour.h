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
        Behaviour(const Actor* actor): mActor(actor) {};
        virtual ~Behaviour() {};

        virtual void update(size_t ticksPassed) = 0;

    protected:
        const Actor * mActor;

    };

    // Does nothing
    class NullBehaviour : public Behaviour
    {
    public:
        NullBehaviour(const Actor* actor): Behaviour(actor) {};
        ~NullBehaviour() {};
        void update(size_t ticksPassed) {
            UNUSED_PARAM(ticksPassed);
        };
    };

    class BasicMonsterBehaviour : public Behaviour
    {
    public:
        BasicMonsterBehaviour(const Actor* actor): Behaviour(actor) {};
        ~BasicMonsterBehaviour() {};
        void update(size_t ticksPassed);

    private:
        size_t mLastActionTick;

        const Player * findNearestPlayer();
    };

}

#endif
