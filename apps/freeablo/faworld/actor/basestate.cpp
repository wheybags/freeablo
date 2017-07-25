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

            FARender::FASpriteGroup* sprite = nullptr;
            int32_t frame;
            actor.getCurrentFrame(sprite, frame);

            if (actor.mMoveHandler.moving() && sprite != actor.mWalkAnim)
                actor.playAnimation(AnimState::walk, FARender::AnimationPlayer::AnimationType::Looped);
            else if (!actor.mMoveHandler.moving() && sprite == actor.mWalkAnim)
                actor.playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);

            return boost::none;
        }
    }
}
