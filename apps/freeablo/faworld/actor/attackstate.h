#pragma once
#include "statemachine.h"
#include <misc/direction.h>
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

            virtual void save(FASaveGame::GameSaver& saver) const override;

            explicit MeleeAttackState(FASaveGame::GameLoader& loader);
            explicit MeleeAttackState(Misc::Direction direction);
            virtual ~MeleeAttackState() = default;
            virtual boost::optional<StateChange> update(Actor& actor, bool noclip) override;

            virtual void onEnter(Actor& actor) override;

        private:
            Misc::Direction mDirection;
            bool mHitDone = false;
        };
    }
}
