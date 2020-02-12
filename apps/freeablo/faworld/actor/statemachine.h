#pragma once
#include <memory>
#include <misc/misc.h>
#include <optional>
#include <stddef.h>
#include <vector>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class Actor;
    struct StateChange;

    enum class StateOperation
    {
        pop,
        push
    };

    class AbstractState
    {
    public:
        virtual void save(FASaveGame::GameSaver& saver) const = 0;
        virtual const std::string& getTypeId() const = 0;
        virtual ~AbstractState() = default;
        virtual std::optional<StateChange> update(Actor& entity, bool noclip) = 0;
        virtual void onEnter(Actor& entity) { UNUSED_PARAM(entity); }
        virtual void onExit(Actor& entity) { UNUSED_PARAM(entity); }
    };

    struct StateChange
    {
        StateChange(StateOperation op) : op(op) {}
        StateChange(StateOperation op, AbstractState* nextState) : op(op), nextState(nextState) {}
        StateOperation op;
        std::unique_ptr<AbstractState> nextState;
    };

    class StateMachine
    {
    public:
        StateMachine(Actor* mEntity, AbstractState* initial = nullptr);
        ~StateMachine();

        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);

        void update(bool noclip);

    private:
        std::vector<std::unique_ptr<AbstractState>> mStateStack;
        Actor* mEntity = nullptr;
    };
}
