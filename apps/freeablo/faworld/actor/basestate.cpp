#include "basestate.h"
#include "../actor.h"
#include "../itemmap.h"
#include "attackstate.h"

namespace FAWorld
{
    namespace ActorState
    {
        const std::string BaseState::typeId = "actor-state-base-state";

        boost::optional<StateChange> BaseState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);
            boost::optional<StateChange> ret;

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
                                actor.attack(target);
                                ret = StateChange{StateOperation::push, new AttackState()};
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
                    if (actor.getPos().isNear({tile.x, tile.y}))
                    {
                        actor.pickupItem(target);
                        actor.mTarget.clear();
                    }
                    else
                    {
                        actor.mMoveHandler.setDestination({tile.x, tile.y}, true);
                    }
                }

                case Target::Type::None:
                    break;
            }

            actor.mMoveHandler.update(actor.getId());

            AnimState anim = actor.mAnimation.getCurrentAnimation();
            if (actor.mMoveHandler.moving() && anim != AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if (!actor.mMoveHandler.moving() && anim == AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            return ret;
        }
    }
}
