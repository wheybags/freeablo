#pragma once
#include "multiplayerinterface.h"
#include <enet/enet.h>

namespace FASaveGame
{
    class GameLoader;
}

namespace Engine
{
    class LocalInputHandler;

    class Client : public MultiplayerInterface
    {
    public:
        Client(LocalInputHandler& localInputHandler);
        virtual ~Client();

        virtual boost::optional<std::vector<FAWorld::PlayerInput>> getAndClearInputs(FAWorld::Tick tick) override;
        virtual void update() override;

    private:
        void processServerPacket(const ENetEvent& event);
        void receiveMap(FASaveGame::GameLoader& loader);
        void receiveInputs(FASaveGame::GameLoader& loader);
        void sendInputs();

        LocalInputHandler& mLocalInputHandler;
        std::unordered_map<FAWorld::Tick, std::vector<FAWorld::PlayerInput>> mInputs;
        ENetHost* mHost = nullptr;
        ENetPeer* mServerPeer = nullptr;
        ENetAddress mAddress = {};
    };
}
