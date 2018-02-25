#pragma once
#include "behaviour.h"
#include "playerinput.h"

namespace FAWorld
{
    class PlayerBehaviour : public Behaviour
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        PlayerBehaviour(FAWorld::Actor* actor);
        PlayerBehaviour() = default;

        virtual void save(FASaveGame::GameSaver&) override {}
        virtual void reAttach(Actor* actor) override;
        virtual void update() override;

        void addInput(const PlayerInput& input);

        virtual ~PlayerBehaviour() {}

        void blockInput();
        void unblockInput();

    private:
        FAWorld::Player* mPlayer = nullptr;
        int32_t mInputBlockedFramesLeft = 0;
        bool mUnblockInput = false;
    };
}
