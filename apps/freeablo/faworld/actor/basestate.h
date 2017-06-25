#ifndef ACTOR_BASESTATE_H
#define ACTOR_BASESTATE_H

#include <stddef.h>
#include <misc/misc.h>
#include <boost/optional.hpp>

namespace FAWorld
{

    class Actor;

    enum StateOperation
    {
        pop,
        push,
        replace
    };

    template<typename state> struct StateChange
    {
        StateChange(StateOperation op):
            op(op), nextState(nullptr) {};
        StateChange(StateOperation op, state* nextState):
            op(op), nextState(nextState) {};
        StateOperation op;
        state* nextState;
    };

    namespace ActorState
    {

        class AttackState;
        class WalkState;

        class BaseState
        {
        public:
            virtual ~BaseState() {};
            virtual boost::optional<StateChange<BaseState>> update(Actor& actor, bool noclip, size_t ticksPassed);
            virtual void onEnter(Actor& actor)
            {
                UNUSED_PARAM(actor);
            };
            virtual void onExit(Actor& actor)
            {
                UNUSED_PARAM(actor);
            };
        };

    }
}

#endif
