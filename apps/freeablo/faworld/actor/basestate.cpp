#include "basestate.h"

#include "../actor.h"
#include "attackstate.h"

namespace FAWorld
{

    namespace ActorState
    {

        boost::optional<StateChange<BaseState>> BaseState::update(Actor& actor, bool noclip, size_t ticksPassed)
        {
            UNUSED_PARAM(ticksPassed);
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
                        return StateChange<BaseState>{StateOperation::push, new AttackState()};
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
                    if (!actor.mPos.mPath.size() || actor.mPos.mIndex == -1 || actor.mPos.mPath.back() != actor.mDestination)
                    {
                        actor.findPath(world.getCurrentLevel(), actor.mPos.mGoal);
                    }
                    if (!actor.mAnimPlaying)
                    {
                        if (actor.mPos.current() == actor.mPos.mGoal)        //the mPos.mGoal maybe changed by the findPath call.so we need judge it again.
                        {
                            actor.mPos.mMoving = false;
                            actor.setAnimation(AnimState::idle);
                        }
                        else
                        {
                            auto nextPos = actor.mPos.pathNext(false);
                            FAWorld::Actor* actorAtNext = world.getActorAt(nextPos.first, nextPos.second);

                            if ((noclip || (actor.mLevel->isPassable(nextPos.first, nextPos.second) &&
                                            (actorAtNext == NULL || actorAtNext == &actor || actorAtNext->isDead()))) && !actor.mAnimPlaying)
                            {
                                if (!actor.mPos.mMoving && !actor.mAnimPlaying)
                                {
                                    actor.mPos.mMoving = true;
                                    actor.setAnimation(AnimState::walk);
                                }
                            }
                            else if (!actor.mAnimPlaying)
                            {
                                actor.mPos.mMoving = false;
                                actor.mDestination = actor.mPos.current();
                                actor.setAnimation(AnimState::idle);
                            }

                            actor.mPos.setDirection(Misc::getVecDir(Misc::getVec(actor.mPos.current(), nextPos)));
                        }
                    }
                }
            }
            else if (actor.mPos.mMoving && actor.mPos.mDist == 0 && !actor.mAnimPlaying)
            {
                actor.mPos.mMoving = false;
                actor.setAnimation(AnimState::idle);
            }

            if (!actor.mIsDead && !actor.mPos.mMoving && !actor.mAnimPlaying && actor.mAnimState != AnimState::idle)
                actor.setAnimation(AnimState::idle);
            else if (!actor.mIsDead && actor.mPos.mMoving && !actor.mAnimPlaying && actor.mAnimState != AnimState::walk)
                actor.setAnimation(AnimState::walk);

            //if walk anim finished,we need move the role to next tiled
            if (!actor.mAnimPlaying) {
                actor.mPos.update();
            }

            return boost::none;
        }

    }

}
