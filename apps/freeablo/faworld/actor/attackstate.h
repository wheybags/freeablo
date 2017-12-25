
#pragma once

#include <misc/misc.h>
#include <statemachine/statemachine.h>
#include <stddef.h>

namespace FAWorld
{

    class Actor;

    namespace ActorState
    {

        class AttackState : public StateMachine::AbstractState<Actor>
        {
        public:
            ~AttackState(){};
            virtual boost::optional<StateMachine::StateChange<Actor>> update(Actor& actor, bool noclip);

            virtual void onEnter(Actor& actor);
        };
    }
}

