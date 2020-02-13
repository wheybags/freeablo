#include "basestate.h"
#include "../actor.h"
#include "../itemmap.h"
#include "attackstate.h"
#include <misc/vec2fix.h>

namespace FAWorld
{
    namespace ActorState
    {
        const std::string BaseState::typeId = "actor-state-base-state";

        std::optional<StateChange> BaseState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);
            std::optional<StateChange> ret;

            if (auto direction = actor.mMeleeAttackRequestedDirection)
            {
                actor.mMeleeAttackRequestedDirection = std::nullopt;
                return StateChange{StateOperation::push, new ActorState::MeleeAttackState(*direction)};
            }

            switch (actor.mTarget.getType())
            {
                case Target::Type::Actor:
                {
                    Actor* target = actor.mTarget.get<Actor*>();

                    if (actor.canInteractWith(target))
                    {
                        // move to the actor, if we're not already on our way
                        if (!actor.getPos().isNear(target->getPos()))
                            actor.mMoveHandler.setDestination(target->getPos().current());
                        else // and interact them if in range
                        {
                            if (actor.canIAttack(target))
                            {
                                auto targetPos = target->getPos().current();
                                auto myPos = actor.getPos().current();
                                ret = StateChange{StateOperation::push,
                                                  new MeleeAttackState(Vec2Fix(targetPos.x - myPos.x, targetPos.y - myPos.y).getDirection())};
                            }
                        }
                    }
                    else
                    {
                        actor.mTarget.clear();
                    }

                    break;
                }

                case Target::Type::Item:
                {
                    Target::ItemTarget target = actor.mTarget.get<Target::ItemTarget>();

                    auto tile = target.item->getTile();
                    if (actor.getPos().isNear(Position(tile.position)))
                    {
                        actor.pickupItem(target);
                        actor.mTarget.clear();
                    }
                    else
                    {
                        actor.mMoveHandler.setDestination(tile.position, true);
                    }

                    break;
                }

                case Target::Type::Door:
                {
                    Misc::Point targetDoor = actor.mTarget.get<Misc::Point>();

                    if (!actor.getLevel()->isDoor(targetDoor))
                    {
                        actor.mTarget.clear();
                        break;
                    }

                    if (actor.getPos().isNear(Position(targetDoor)))
                    {
                        actor.getLevel()->activateDoor(targetDoor);
                        actor.mTarget.clear();
                    }
                    else
                    {
                        actor.mMoveHandler.setDestination(targetDoor, true);
                    }

                    break;
                }

                case Target::Type::None:
                    break;
            }

            actor.mMoveHandler.update(actor);
            debug_assert(actor.getLevel()->isPassable(actor.mMoveHandler.getCurrentPosition().next(), &actor));

            AnimState anim = actor.mAnimation.getCurrentAnimation();
            if (actor.mMoveHandler.moving() && anim != AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if (!actor.mMoveHandler.moving() && anim == AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            return ret;
        }
    }
}
