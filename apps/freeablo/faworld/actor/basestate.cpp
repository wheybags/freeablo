#include "basestate.h"

#include "../actor.h"
#include "attackstate.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateMachine::StateChange<Actor>> BaseState::update(Actor& actor, bool noclip)
        {
            UNUSED_PARAM(noclip);

            if (actor.actorTarget != nullptr)
            {
                if (actor.canIAttack(actor.actorTarget))
                {
                    int32_t attackDistance = 2;
                    auto currentDest = actor.mMoveHandler.getDestination();

                    // move to the actor, if we're not already on our way
                    if(actor.actorTarget->getPos().distanceFrom(Position(currentDest.first, currentDest.second)) > attackDistance)
                        actor.mMoveHandler.setDestination(actor.actorTarget->getPos().current()); 

                    // and attack them if in range
                    bool inRange = actor.getPos().distanceFrom(actor.actorTarget->getPos()) <= attackDistance;
                    if (inRange && actor.attack(actor.actorTarget))
                        return StateMachine::StateChange<Actor>{StateMachine::StateOperation::push, new AttackState()};
                }
                else
                {
                    actor.actorTarget = nullptr;
                }
            }

            actor.mMoveHandler.update();

            AnimState anim = actor.mAnimation.getCurrentAnimation();

            if (actor.mMoveHandler.moving() && anim != AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if (!actor.mMoveHandler.moving() && anim == AnimState::walk)
                actor.mAnimation.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            return boost::none;
        }
    }
}
