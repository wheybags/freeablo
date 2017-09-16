#include "basestate.h"

#include "../actor.h"
#include "attackstate.h"
#include "../ItemMap.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateMachine::StateChange<Actor>> BaseState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);
            boost::optional<StateMachine::StateChange<Actor>> ret;

            auto done = boost::apply_visitor (Misc::overload<bool>([](boost::blank){ return false; },
                [&actor, &ret](Actor * target)
            {
                Position pos;
                if (actor.canInteractWith(target))
                {
                    int32_t interactionDistance = 2;
                    auto currentDest = actor.mMoveHandler.getDestination();

                    // move to the actor, if we're not already on our way
                    if(target->getPos().distanceFrom(Position(currentDest.first, currentDest.second)) > interactionDistance)
                        actor.mMoveHandler.setDestination(target->getPos().current());

                    // and attack them if in range
                    bool inRange = actor.getPos().manhattanDistance(target->getPos()) <= interactionDistance;
                    if (inRange)
                        {
                            if (actor.canIAttack(target) && actor.attack(target))
                            {
                              ret = StateMachine::StateChange<Actor>{StateMachine::StateOperation::push, new AttackState()};
                              return true;
                            }
                            else if (actor.canTalkTo(target) && actor.talk(target)) {

                            }
                        }
                }
                else
                {
                    actor.setTarget (boost::blank{});
                }
                return true;
            },
            [&actor](PlacedItemData *item)
            {
              auto tile = item->getTile ();
              if (actor.getPos().manhattanDistance ({tile.x, tile.y}) <= 2)
                  {
                      actor.pickupItem(item);
                      actor.setTarget (boost::blank{});
                  }
              else
                  actor.mMoveHandler.setDestination({tile.x, tile.y}, true);
              return true;
            }), actor.mTarget);

            actor.mMoveHandler.update(actor.getId());

            if (done)
              return ret;

            AnimState anim = actor.mAnimation.getCurrentAnimation();

            if (actor.mMoveHandler.moving() && anim != AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if (!actor.mMoveHandler.moving() && anim == AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            return boost::none;
        }
    }
}
