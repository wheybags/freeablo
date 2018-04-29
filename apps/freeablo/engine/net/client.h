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
        virtual ~Client() override;

        virtual boost::optional<std::vector<FAWorld::PlayerInput>> getAndClearInputs(FAWorld::Tick tick) override;
        virtual void update() override;
        virtual void verify(FAWorld::Tick tick) override;
        virtual bool isServer() const override { return false; }

    private:
        void processServerPacket(const ENetEvent& event);
        void receiveMap(FASaveGame::GameLoader& loader);
        void receiveInputs(FASaveGame::GameLoader& loader);
        void sendClientUpdate();

        LocalInputHandler& mLocalInputHandler;
        std::unordered_map<FAWorld::Tick, std::vector<FAWorld::PlayerInput>> mInputs;

        bool mDoFullVerify = false;
        std::unordered_map<FAWorld::Tick, std::string> mServerStatesForFullVerify;

        ENetHost* mHost = nullptr;
        ENetPeer* mServerPeer = nullptr;
        ENetAddress mAddress;
    };
}
