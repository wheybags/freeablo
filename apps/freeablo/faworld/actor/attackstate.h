
#pragma once

#include "statemachine.h"
#include <misc/misc.h>
#include <stddef.h>

namespace FAWorld
{
    class Actor;

    namespace ActorState
    {
        class MeleeAttackState : public AbstractState
        {
        public:
            static const std::string typeId;
            const std::string& getTypeId() const override { return typeId; }

            virtual void save(FASaveGame::GameSaver&) const override {}

            explicit MeleeAttackState(int direction);
            virtual ~AttackState() = default;
            virtual boost::optional<StateChange> update(Actor& actor, bool noclip);

            virtual void onEnter(Actor& actor);

        private:
            int32_t mDirection;
        };
    }
}
