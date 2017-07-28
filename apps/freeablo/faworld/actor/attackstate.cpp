#include "attackstate.h"
#include "../actor.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateMachine::StateChange<Actor>> AttackState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);

            if (actor.getAnimationManager().getCurrentAnimation() != AnimState::attack)
                return StateMachine::StateChange<Actor>{StateMachine::StateOperation::pop};

            return boost::none;
        }

        void AttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            actor.getAnimationManager().playAnimation(AnimState::attack, FARender::AnimationPlayer::AnimationType::Once);
        }
    }
}
