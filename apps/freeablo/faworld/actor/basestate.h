
#pragma once

#include "statemachine.h"
#include <boost/optional.hpp>
#include <misc/misc.h>
#include <stddef.h>

namespace FAWorld
{

    class Actor;

    namespace ActorState
    {
        class BaseState : public AbstractState
        {
        public:
            ~BaseState() = default;

            static const std::string typeId;
            const std::string& getTypeId() const override { return typeId; }

            virtual void save(FASaveGame::GameSaver&) const override {}

            virtual boost::optional<StateChange> update(Actor& actor, bool noclip) override;
        };
    }
}
