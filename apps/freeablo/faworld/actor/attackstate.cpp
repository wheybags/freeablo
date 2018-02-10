#include "attackstate.h"
#include "../actor.h"

namespace FAWorld
{

    namespace ActorState
    {
        const std::string MeeleeAttackState::typeId = "actorstate-attack-state";

        MeleeAttackState::MeleeAttackState(Misc::Direction direction) : mDirection(direction) {}

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
            if (!mHitDone && // to fix the problem with several updates during a single frame
                actor.mAnimation.getCurrentRealFrame().second == attackFrame)
            {
                actor.doMeleeHit(Misc::getNextPosByDir(actor.getPos().current(), mDirection));
                mHitDone = true;
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
