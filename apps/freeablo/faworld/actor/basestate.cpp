#include "basestate.h"

#include "../actor.h"
#include "attackstate.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateMachine::StateChange<Actor>> BaseState::update(Actor& actor, bool noclip)
        {
            if (actor.mPos.mGoal != std::pair<int32_t, int32_t>(0, 0) && actor.mPos.current() != actor.mPos.mGoal)
            {
                Actor * destActor;
                destActor = World::get()->getActorAt(actor.mDestination.first, actor.mDestination.second);
                if (actor.canIAttack(destActor))
                {
                    std::pair<float, float> vector = Misc::getVec(actor.mPos.current(), actor.mDestination);
                    actor.mPos.setDirection(Misc::getVecDir(vector));
                    actor.mPos.update();
                    actor.mPos.mDist = 0;

                    if (actor.attack(destActor)) {
                        return StateMachine::StateChange<Actor>{StateMachine::StateOperation::push, new AttackState()};
                    }
                }
                else if (actor.canTalkTo(destActor))
                {
                    actor.mPos.mDist = 0;
                    actor.talk(destActor);
                }
                else if (actor.mPos.mDist == 0)
                {
                    World& world = *World::get();
                    actor.findPath(world.getCurrentLevel(), actor.mPos.mGoal);
                    
                    auto nextPos = actor.mPos.pathNext(false);
                    FAWorld::Actor* actorAtNext = world.getActorAt(nextPos.first, nextPos.second);

                    if ((noclip || (actor.mLevel->isPassable(nextPos.first, nextPos.second) &&
                                    (actorAtNext == NULL || actorAtNext == &actor || actorAtNext->isDead()))))
                    {
                        actor.mPos.setDirection(Misc::getVecDir(Misc::getVec(actor.mPos.current(), nextPos)));
                        actor.mPos.mMoving = true;
                    }
                    else
                    {
                        actor.mPos.mGoal = actor.mPos.current();
                    }
                }
            }

            if (actor.mPos.current() == actor.mPos.mGoal)
                actor.mPos.mMoving = false;


            FARender::FASpriteGroup* sprite = nullptr;
            int32_t frame;
            actor.getCurrentFrame(sprite, frame);

            if(actor.mPos.mMoving && sprite == actor.mIdleAnim)
                actor.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if(!actor.mPos.mMoving && sprite == actor.mWalkAnim)
                actor.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            actor.mPos.update();

            return boost::none;
        }

    }

}
