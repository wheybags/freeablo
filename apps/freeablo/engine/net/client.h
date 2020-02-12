#pragma once
#include "multiplayerinterface.h"
#include <enet/enet.h>
#include <set>

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
        Client(LocalInputHandler& localInputHandler, const std::string& serverAddress);
        virtual ~Client() override;

        virtual std::optional<std::vector<FAWorld::PlayerInput>> getAndClearInputs(FAWorld::Tick tick) override;
        virtual void update() override;
        virtual void verify(FAWorld::Tick tick) override;
        virtual bool isServer() const override { return false; }
        virtual bool isMultiplayer() const override { return true; }
        virtual bool isPlayerRegistered(uint32_t peerId) const override;
        virtual void registerNewPlayer(FAWorld::Player*, uint32_t peerId) override;

        bool isConnected() { return mConnected; }
        bool didConnectionFail() { return mConnectionFailed; }

    private:
        void processServerPacket(const ENetEvent& event);
        void receiveMap(FASaveGame::GameLoader& loader);
        void receiveInputs(FASaveGame::GameLoader& loader);
        void receiveVerifyPacket(FASaveGame::GameLoader& loader);
        void sendClientUpdate();

        std::set<uint32_t> mRegisteredClientIds;

        FAWorld::Tick mLastTickISentInputsOn = 0;
        LocalInputHandler& mLocalInputHandler;
        uint32_t mLastLocalInputId = 0;
        std::map<uint32_t, std::vector<FAWorld::PlayerInput>> mLocalInputsBuffer;

        std::unordered_map<FAWorld::Tick, std::vector<FAWorld::PlayerInput>> mInputs;

        bool mDoFullVerify = false;
        std::unordered_map<FAWorld::Tick, std::string> mServerStatesForFullVerify;

        ENetHost* mHost = nullptr;
        ENetPeer* mServerPeer = nullptr;
        ENetAddress mAddress;

        bool mConnected = false;
        bool mConnectionFailed = false;

        static constexpr size_t MAX_CLIENT_UPDATE_PACKET_SIZE = 500;
    };
}
