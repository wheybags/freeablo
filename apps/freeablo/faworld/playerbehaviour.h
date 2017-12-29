
#pragma once

#include "behaviour.h"
#include "../engine/inputobserverinterface.h"

namespace FAWorld
{
    class PlayerBehaviour : public Behaviour, public Engine::MouseInputObserverInterface
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        PlayerBehaviour(FAWorld::Actor* actor);
        PlayerBehaviour() = default;

        virtual void save(FASaveGame::GameSaver&) override {}
        virtual void reAttach(Actor* actor) override;
        virtual void update() override;

        virtual void notify(Engine::MouseInputAction action, Misc::Point mousePosition, bool mouseDown) override;

        virtual ~PlayerBehaviour() {}

        void blockInput();
        void unblockInput();

    private:
        FAWorld::Player* mPlayer = nullptr;
        int32_t mInputBlockedFramesLeft = 0;
        bool mUnblockInput = false;
        bool mTargetLock = false;
    };
}
