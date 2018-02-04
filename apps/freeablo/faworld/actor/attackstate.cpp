#include "attackstate.h"
#include "../actor.h"

namespace FAWorld
{

    namespace ActorState
    {
        const std::string MeeleeAttackState::typeId = "actorstate-attack-state";

        MeleeAttackState::MeleeAttackState(int direction) : mDirection(direction) {}

        boost::optional<StateChange> MeleeAttackState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);

            auto& animManager = actor.mAnimation;
            if (animManager.getCurrentAnimation() != AnimState::attack && animManager.getInterruptedAnimation() != AnimState::attack)
            {
                actor.isAttacking = false;
                return StateChange{StateOperation::pop};
            }

            // NOTE: this is approximation
            // in reality attack frame differs for each weapon for player
            // and most likely also specified exactly for each monster
            auto attackFrame = actor.mAnimation.getCurrentAnimationLength() / 2;
            if (actor.mAnimation.getCurrentRealFrame().second == attackFrame)
            {
                actor.attack(Misc::getNextPosByDir(actor.getPos().current(), mDirection));
            }

            return boost::none;
        }

        void MeleeAttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            actor.setDirection(mDirection);
            actor.mAnimation.playAnimation(AnimState::attack, FARender::AnimationPlayer::AnimationType::Once);
        }
    }
}
