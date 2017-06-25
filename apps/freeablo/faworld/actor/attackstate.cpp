#include "attackstate.h"
#include "../actor.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateChange<BaseState>> AttackState::update(Actor& actor, bool noclip, size_t ticksPassed)
        {
            UNUSED_PARAM(noclip);
            UNUSED_PARAM(ticksPassed);

            if (!actor.mAnimPlaying) {
                actor.setAnimation(AnimState::idle);
                return StateChange<BaseState>{StateOperation::pop};
            }

            return boost::none;
        }

        void AttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            actor.setAnimation(AnimState::attack, true);
            actor.mAnimPlaying = true;
        }

    }

}
