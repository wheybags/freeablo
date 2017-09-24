#include "basestate.h"

#include "../actor.h"
#include "attackstate.h"
#include "../itemmap.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateMachine::StateChange<Actor>> BaseState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);
            boost::optional<StateMachine::StateChange<Actor>> ret;

            boost::apply_visitor (Misc::overload([](boost::blank){},
                [&actor, &ret](Actor * target)
            {
                Position pos;
                if (actor.canInteractWith(target))
                {
                    // move to the actor, if we're not already on our way
                    if(!actor.getPos().isNear(target->getPos()))
                        actor.mMoveHandler.setDestination(target->getPos().current());
                    else // and interact them if in range
                        {
                            if (actor.canIAttack(target) && actor.attack(target))
                            {
                              ret = StateMachine::StateChange<Actor>{StateMachine::StateOperation::push, new AttackState()};
                            }
                            else if (actor.canTalkTo(target) && actor.talk(target)) {

                            }
                        }
                }
                else
                {
                    actor.setTarget (boost::blank{});
                }
            },
            [&actor](const ItemTarget &target)
            {
              auto tile = target.item->getTile ();
              if (actor.getPos().isNear ({tile.x, tile.y}))
                  {
                      actor.pickupItem(target);
                      actor.setTarget (boost::blank{});
                  }
              else
                  actor.mMoveHandler.setDestination({tile.x, tile.y}, true);
            }), actor.mTarget);

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
