#include "statemachine.h"
#include "../../fasavegame/gameloader.h"
#include "../world.h"

namespace FAWorld
{
    StateMachine::StateMachine(Actor* mEntity, AbstractState* initial) : mEntity(mEntity)
    {
        if (initial)
            mStateStack.emplace_back(initial);
    }

    void StateMachine::save(FASaveGame::GameSaver& saver) const
    {
        uint32_t stackSize = uint32_t(mStateStack.size());
        saver.save(stackSize);
        for (const auto& state : mStateStack)
        {
            saver.save(state->getTypeId());
            state->save(saver);
        }
    }

    void StateMachine::load(FASaveGame::GameLoader& loader)
    {
        uint32_t stackSize = loader.load<uint32_t>();
        for (uint32_t i = 0; i < stackSize; i++)
        {
            std::string typeId = loader.load<std::string>();
            auto state = static_cast<AbstractState*>(FAWorld::World::get()->mObjectIdMapper.construct(typeId, loader));
            mStateStack.emplace_back(state);
        }
    }

    StateMachine::~StateMachine() = default;

    void StateMachine::update(bool noclip)
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
                        mStateStack.emplace_back(next->nextState.release());
                        break;
                }

                mStateStack.back()->onEnter(*mEntity);
            }
        }
    }
}
