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

        std::optional<StateChange> MeleeAttackState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);

            auto& animManager = actor.mAnimation;
            if (animManager.getCurrentAnimation() != AnimState::attack)
            {
                actor.isAttacking = false;
                return StateChange{StateOperation::pop};
            }

            int32_t attackFrame = actor.getMeleeHitFrame();
            if (!mHitDone && // to fix the problem with several updates during a single frame
                actor.mAnimation.getCurrentRealFrame().second == attackFrame)
            {
                actor.doMeleeHit(Misc::getNextPosByDir(actor.getPos().current(), mDirection));
                mHitDone = true;
            }

            return std::nullopt;
        }

        void MeleeAttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            actor.stopMoving(mDirection);
            actor.mAnimation.playAnimation(AnimState::attack, FARender::AnimationPlayer::AnimationType::Once);
        }
    }
}
