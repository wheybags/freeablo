#include "attackstate.h"
#include "../../fasavegame/gameloader.h"
#include "../actor.h"

namespace FAWorld
{

    namespace ActorState
    {
        const std::string MeleeAttackState::typeId = "actorstate-attack-state";

        void MeleeAttackState::save(FASaveGame::GameSaver& saver) const { mDirection.save(saver); }

        MeleeAttackState::MeleeAttackState(FASaveGame::GameLoader& loader) { mDirection = Misc::Direction(loader); }

        MeleeAttackState::MeleeAttackState(Misc::Direction direction) : mDirection(direction) {}

        nonstd::optional<StateChange> MeleeAttackState::update(Actor& actor, bool noclip)
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

            return nonstd::nullopt;
        }

        void MeleeAttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            actor.stopAndPointInDirection(mDirection);
            actor.mAnimation.playAnimation(AnimState::attack, FARender::AnimationPlayer::AnimationType::Once);
        }
    }
}
