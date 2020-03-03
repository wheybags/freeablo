#include "attackstate.h"
#include "../../fasavegame/gameloader.h"
#include "../actor.h"
#include "../spells.h"

namespace FAWorld
{
    namespace ActorState
    {
        void BaseAttackState::save(FASaveGame::GameSaver& saver) const
        {
            mTargetPoint.save(saver);
            saver.save(mAttackDone);
        }

        BaseAttackState::BaseAttackState(FASaveGame::GameLoader& loader)
        {
            mTargetPoint = Misc::Point(loader);
            mAttackDone = loader.load<bool>();
        }

        BaseAttackState::BaseAttackState(Misc::Point targetPoint) : mTargetPoint(targetPoint) {}

        std::optional<StateChange> BaseAttackState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);

            auto& animManager = actor.mAnimation;
            if (animManager.getCurrentAnimation() != getAnimation())
            {
                actor.isAttacking = false;
                return StateChange{StateOperation::pop};
            }

            int32_t attackFrame = getAttackFrame(actor);
            if (!mAttackDone && // to fix the problem with several updates during a single frame
                actor.mAnimation.getCurrentRealFrame().second == attackFrame)
            {
                doAttack(actor);
                mAttackDone = true;
            }

            return std::nullopt;
        }

        void BaseAttackState::onEnter(Actor& actor)
        {
            actor.isAttacking = true;
            auto actorPos = actor.getPos().current();
            auto direction = Vec2Fix(mTargetPoint.x - actorPos.x, mTargetPoint.y - actorPos.y).getDirection();
            actor.stopMoving(direction);
            actor.mAnimation.playAnimation(getAnimation(), FARender::AnimationPlayer::AnimationType::Once);
        }

        const std::string MeleeAttackState::typeId = "actorstate-melee-attack-state";

        void MeleeAttackState::doAttack(Actor& actor)
        {
            // Melee can only attack the nearest position/tile in a direction.
            auto actorPos = actor.getPos().current();
            auto direction = Vec2Fix(mTargetPoint.x - actorPos.x, mTargetPoint.y - actorPos.y).getDirection();
            auto pos = getNextPosByDir(actorPos, direction);
            actor.doMeleeHit(pos);
        }

        int32_t MeleeAttackState::getAttackFrame(Actor& actor) const { return actor.getMeleeHitFrame(); }

        const std::string RangedAttackState::typeId = "actorstate-ranged-attack-state";

        void RangedAttackState::doAttack(Actor& actor) { actor.doRangedAttack(mTargetPoint); }

        const std::string SpellAttackState::typeId = "actorstate-spell-attack-state";

        void SpellAttackState::save(FASaveGame::GameSaver& saver) const
        {
            BaseAttackState::save(saver);
            saver.save((int32_t)mSpell);
        }

        SpellAttackState::SpellAttackState(FASaveGame::GameLoader& loader) : BaseAttackState(loader) { mSpell = (SpellId)loader.load<int32_t>(); }

        void SpellAttackState::doAttack(Actor& actor) { actor.doSpellEffect(mSpell, mTargetPoint); }

        AnimState SpellAttackState::getAnimation() const
        {
            auto spellData = SpellData(mSpell);
            switch (spellData.getType())
            {
                case DiabloExe::SpellData::SpellType::fire:
                    return AnimState::spellFire;
                case DiabloExe::SpellData::SpellType::lightning:
                    return AnimState::spellLightning;
                case DiabloExe::SpellData::SpellType::magic:
                default:
                    return AnimState::spellOther;
            }
        }

        int32_t SpellAttackState::getAttackFrame(Actor& actor) const
        {
            // Initiate spell on the last frame.
            return actor.mAnimation.getAnimationSprites(getAnimation())->getAnimLength() - 1;
        }
    }
}
