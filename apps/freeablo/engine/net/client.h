#pragma once
#include "multiplayerinterface.h"
#include <enet/enet.h>

namespace FASaveGame
{
    class GameLoader;
}

namespace Engine
{
    class Client : public MultiplayerInterface
    {
    public:
        Client();
        virtual ~Client();

        virtual std::vector<FAWorld::PlayerInput> getAndClearInputs() override;
        virtual void update() override;

    private:
        void processServerPacket(const ENetEvent& event);
        void receiveMap(FASaveGame::GameLoader& loader);

        ENetHost* mHost = nullptr;
        ENetPeer* mServerPeer = nullptr;
        ENetAddress mAddress = {};
    };
}
