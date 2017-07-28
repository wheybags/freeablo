#ifndef ACTOR_ATTACKSTATE_H
#define ACTOR_ATTACKSTATE_H

#include <stddef.h>
#include <statemachine/statemachine.h>
#include <misc/misc.h>
#include <boost/optional.hpp>

namespace FAWorld
{

    class Actor;

    namespace ActorState
    {

        class AttackState : public StateMachine::AbstractState<Actor>
        {
        public:
            ~AttackState() {};
            virtual boost::optional<StateMachine::StateChange<Actor>> update(Actor& actor, bool noclip);

            virtual void onEnter(Actor& actor);
        };

    }

}

#endif
