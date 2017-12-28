
#pragma once

#include <boost/optional.hpp>
#include <misc/misc.h>
#include <statemachine/statemachine.h>
#include <stddef.h>

namespace FAWorld
{

    class Actor;

    namespace ActorState
    {

        class BaseState : public StateMachine::AbstractState<Actor>
        {
        public:
            ~BaseState(){};
            virtual boost::optional<StateMachine::StateChange<Actor>> update(Actor& actor, bool noclip);
        };
    }
}
