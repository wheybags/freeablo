
#pragma once

#include "statemachine.h"
#include <misc/misc.h>
#include <stddef.h>

namespace FAWorld
{
    class Actor;

    namespace ActorState
    {
        class AttackState : public AbstractState
        {
        public:
            static const std::string typeId;
            const std::string& getTypeId() const override { return typeId; }

            virtual void save(FASaveGame::GameSaver&) const override {}

            virtual ~AttackState() = default;
            virtual boost::optional<StateChange> update(Actor& actor, bool noclip);

            virtual void onEnter(Actor& actor);
        };
    }
}
