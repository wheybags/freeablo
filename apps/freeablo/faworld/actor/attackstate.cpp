#include "attackstate.h"
#include "../actor.h"

namespace FAWorld
{

    namespace ActorState
    {
        const std::string AttackState::typeId = "actorstate-attack-state";

        boost::optional<StateChange> AttackState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);

            auto& animManager = actor.mAnimation;
            if (animManager.getCurrentAnimation() != AnimState::attack && animManager.getInterruptedAnimation() != AnimState::attack)
            {
                actor.isAttacking = false;
                return StateChange{StateOperation::pop};
            }

            return boost::none;
        }

        void AttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            actor.mAnimation.playAnimation(AnimState::attack, FARender::AnimationPlayer::AnimationType::Once);
        }
    }
}
