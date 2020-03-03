#pragma once
#include "faworld/actoranimationmanager.h"
#include "statemachine.h"
#include <misc/direction.h>
#include <misc/misc.h>
#include <stddef.h>

namespace FAWorld
{
    class Actor;

    namespace ActorState
    {
        class BaseAttackState : public AbstractState
        {
        public:
            virtual void save(FASaveGame::GameSaver& saver) const override;

            explicit BaseAttackState(FASaveGame::GameLoader& loader);
            explicit BaseAttackState(Misc::Point targetPoint);
            virtual ~BaseAttackState() = default;
            virtual std::optional<StateChange> update(Actor& actor, bool noclip) override;

            virtual void onEnter(Actor& actor) override;

        protected:
            virtual void doAttack(Actor& actor) = 0;
            virtual AnimState getAnimation() const = 0;
            virtual int32_t getAttackFrame(Actor& actor) const = 0;

            Misc::Point mTargetPoint;
            bool mAttackDone = false;
        };

        class MeleeAttackState : public BaseAttackState
        {
        public:
            static const std::string typeId;
            const std::string& getTypeId() const override { return typeId; }

            explicit MeleeAttackState(FASaveGame::GameLoader& loader) : BaseAttackState(loader) {}
            explicit MeleeAttackState(Misc::Point targetPoint) : BaseAttackState(targetPoint) {}

        protected:
            void doAttack(Actor& actor) override;
            AnimState getAnimation() const override { return AnimState::attack; };
            int32_t getAttackFrame(Actor& actor) const override;
        };

        class RangedAttackState : public MeleeAttackState
        {
        public:
            static const std::string typeId;
            const std::string& getTypeId() const override { return typeId; }

            explicit RangedAttackState(FASaveGame::GameLoader& loader) : MeleeAttackState(loader) {}
            explicit RangedAttackState(Misc::Point targetPoint) : MeleeAttackState(targetPoint) {}

        protected:
            void doAttack(Actor& actor) override;
        };

        class SpellAttackState : public BaseAttackState
        {
        public:
            static const std::string typeId;
            const std::string& getTypeId() const override { return typeId; }

            void save(FASaveGame::GameSaver& saver) const override;

            explicit SpellAttackState(FASaveGame::GameLoader& loader);
            explicit SpellAttackState(SpellId spell, Misc::Point targetPoint) : BaseAttackState(targetPoint), mSpell(spell) {}

        protected:
            void doAttack(Actor& actor) override;
            AnimState getAnimation() const override;
            int32_t getAttackFrame(Actor& actor) const override;

            SpellId mSpell;
        };
    }
}
