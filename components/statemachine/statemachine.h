#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <misc/misc.h>
#include <vector>
#include <stddef.h>
#include <boost/optional.hpp>

namespace StateMachine
{

    template <typename E> struct StateChange;

    enum StateOperation
    {
        pop,
        push,
        replace
    };

    template<typename E>
    class AbstractState
    {
    public:
        virtual ~AbstractState() {};
        virtual boost::optional<StateChange<E>> update(E& entity, bool noclip) = 0;
        virtual void onEnter(E& entity)
        {
                UNUSED_PARAM(entity);
        };
        virtual void onExit(E& entity)
        {
                UNUSED_PARAM(entity);
        };
    };

    template <typename E>
    struct StateChange
    {
        StateChange(StateOperation op):
            op(op), nextState(nullptr) {};
        StateChange(StateOperation op, AbstractState<E>* nextState):
            op(op), nextState(nextState) {};
        StateOperation op;
        AbstractState<E>* nextState;
    };

    template <typename E>
    class StateMachine
    {
    public:
        StateMachine(AbstractState<E>* initial, E* mEntity): mEntity(mEntity) {
            mStateStack.push_back(initial);
        }
        ~StateMachine() {};

        void update(bool noclip)
        {
            if (!mStateStack.empty()) {
                if (auto next = mStateStack.back()->update(*mEntity, noclip)) {
                    mStateStack.back()->onExit(*mEntity);

                    switch (next->op) {
                    case StateOperation::pop:
                        mStateStack.pop_back();
                        break;
                    case StateOperation::push:
                        mStateStack.push_back(next->nextState);
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

#endif
