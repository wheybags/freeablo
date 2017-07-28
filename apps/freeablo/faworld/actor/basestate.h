#ifndef ACTOR_BASESTATE_H
#define ACTOR_BASESTATE_H

#include <stddef.h>
#include <statemachine/statemachine.h>
#include <misc/misc.h>
#include <boost/optional.hpp>

namespace FAWorld
{

    class Actor;

    namespace ActorState
    {

        class BaseState: public StateMachine::AbstractState<Actor>
        {
        public:
            ~BaseState() {};
            virtual boost::optional<StateMachine::StateChange<Actor>> update(Actor& actor, bool noclip);
        };

    }
}

#endif
