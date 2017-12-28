
#pragma once

#include <boost/optional.hpp>
#include <misc/misc.h>
#include <stddef.h>
#include <vector>

namespace StateMachine
{

    template <typename E> struct StateChange;

    enum class StateOperation
    {
        pop,
        push,
        replace
    };

    template <typename E> class AbstractState
    {
    public:
        virtual ~AbstractState(){};
        virtual boost::optional<StateChange<E>> update(E& entity, bool noclip) = 0;
        virtual void onEnter(E& entity) { UNUSED_PARAM(entity); };
        virtual void onExit(E& entity) { UNUSED_PARAM(entity); };
    };

    template <typename E> struct StateChange
    {
        StateChange(StateOperation op) : op(op), nextState(nullptr){};
        StateChange(StateOperation op, AbstractState<E>* nextState) : op(op), nextState(nextState){};
        StateOperation op;
        AbstractState<E>* nextState;
    };

    template <typename E> class StateMachine
    {
    public:
        StateMachine(AbstractState<E>* initial, E* mEntity) : mEntity(mEntity) { mStateStack.push_back(initial); }
        ~StateMachine(){};

        void update(bool noclip)
        {
            if (!mStateStack.empty())
            {
                if (auto next = mStateStack.back()->update(*mEntity, noclip))
                {
                    mStateStack.back()->onExit(*mEntity);

                    switch (next->op)
                    {
                        case StateOperation::pop:
                            mStateStack.pop_back();
                            break;
                        case StateOperation::push:
                            mStateStack.push_back(next->nextState);
                            break;
                        case StateOperation::replace:
                            mStateStack.pop_back();
                            mStateStack.push_back(next->nextState);
                            break;
                    }

                    mStateStack.back()->onEnter(*mEntity);
                }
            }
        }

    private:
        std::vector<AbstractState<E>*> mStateStack;
        E* mEntity;
    };
}
