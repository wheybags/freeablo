#include "basestate.h"

#include "../actor.h"
#include "attackstate.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateMachine::StateChange<Actor>> BaseState::update(Actor& actor, bool noclip)
        {
            actor.mMoveHandler.update();

            /*if (actor.getPos().mGoal != std::pair<int32_t, int32_t>(0, 0) && actor.getPos().current() != actor.getPos().mGoal)
            {
                Actor * destActor;
                destActor = World::get()->getActorAt(actor.mDestination.first, actor.mDestination.second);
                if (actor.canIAttack(destActor))
                {
                    std::pair<float, float> vector = Misc::getVec(actor.getPos().current(), actor.mDestination);
                    actor.getPos().setDirection(Misc::getVecDir(vector));
                    actor.getPos().update();
                    actor.getPos().mDist = 0;

                    if (actor.attack(destActor)) {
                        return StateMachine::StateChange<Actor>{StateMachine::StateOperation::push, new AttackState()};
                    }
                }
                else if (actor.canTalkTo(destActor))
                {
                    actor.getPos().mDist = 0;
                    actor.talk(destActor);
                }
                else if (actor.getPos().mDist == 0)
                {
                    World& world = *World::get();
                    actor.findPath(world.getCurrentLevel(), actor.getPos().mGoal);
                    
                    auto nextPos = actor.getPos().pathNext(false);

                    if (actor.mLevel->isPassable(nextPos.first, nextPos.second))
                    {
                        actor.getPos().setDirection(Misc::getVecDir(Misc::getVec(actor.getPos().current(), nextPos)));
                        actor.getPos().mMoving = true;
                    }
                    else
                    {
                        actor.getPos().mGoal = actor.getPos().current();
                    }
                }
            }

            if (actor.getPos().current() == actor.getPos().mGoal)
                actor.getPos().mMoving = false;


            FARender::FASpriteGroup* sprite = nullptr;
            int32_t frame;
            actor.getCurrentFrame(sprite, frame);

            if(actor.getPos().mMoving && sprite != actor.mWalkAnim)
                actor.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if(!actor.getPos().mMoving && sprite == actor.mWalkAnim)
                actor.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            actor.getPos().update();*/

            return boost::none;
        }

    }

}
