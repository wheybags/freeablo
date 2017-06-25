#ifndef ACTOR_ATTACKSTATE_H
#define ACTOR_ATTACKSTATE_H

#include "basestate.h"

namespace FAWorld
{

    class Actor;

    namespace ActorState
    {

        class AttackState : public BaseState
        {
        public:
            ~AttackState() {};
            boost::optional<StateChange<BaseState>> update(Actor& actor, bool noclip, size_t ticksPassed);

            void onEnter(Actor& actor);
        };

    }

}

#endif
