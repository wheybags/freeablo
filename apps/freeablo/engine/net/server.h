#pragma once
#include "multiplayerinterface.h"
#include <enet/enet.h>
#include <misc/averager.h>
#include <unordered_map>

namespace FAWorld
{
    class World;
}

namespace Engine
{
    class LocalInputHandler;

    class Server : public MultiplayerInterface
    {
    public:
        Server(FAWorld::World& world, LocalInputHandler& localInputHandler);
        virtual ~Server();

        virtual std::optional<std::vector<FAWorld::PlayerInput>> getAndClearInputs(FAWorld::Tick tick) override;
        virtual void update() override;
        virtual void verify(FAWorld::Tick) override {}
        virtual bool isServer() const override { return true; }
        virtual bool isMultiplayer() const override { return !mPeers.empty(); }
        virtual bool isPlayerRegistered(uint32_t peerId) const override;
        virtual void registerNewPlayer(FAWorld::Player* player, uint32_t peerId) override;
        virtual void doMultiplayerGui(nk_context* ctx) override;

    private:
        struct Peer
        {
            Peer() = default;
            explicit Peer(ENetPeer* peer) : peer(peer) {}

            ENetPeer* peer = nullptr;
            bool hasMap = false;
            bool mapSent = false;

            FAWorld::Tick lastTick = 0;
            uint32_t lastInputSetIdReceived = 0;

            int32_t actorId = -1;
            size_t bytesSentLastTick = 0;
            FAWorld::Tick lastSentTick = -1;
        };

        void handleMapSending();
        void handleEvents();
        void processInputs();
        void onPeerConnect(const ENetEvent& event);
        void onPeerDisconnect(const ENetEvent& event);
        void sendMapToPeer(Peer& peer);
        void readPeerPacket(const ENetEvent& event);
        void sendInputsToClients(std::vector<FAWorld::PlayerInput>& inputs);
        void receiveClientUpdate(FASaveGame::GameLoader& loader, Peer& peer);

        static const char* SERVER_ADDRESS;

        bool mDoFullVerify = false;
        FAWorld::Tick mLastTickVerified = -1;

        FAWorld::World& mWorld;
        LocalInputHandler& mLocalInputHandler;

        // This is where we accumulate inputs received from clients before we execute them
        std::vector<FAWorld::PlayerInput> mInputsBuffer;

        // Inputs from old ticks that clients might not have yet
        std::unordered_map<FAWorld::Tick, std::vector<FAWorld::PlayerInput>> mOldInputs;

        // This is where we locally store inputs to be executed by the server.
        // They are accumulated in mInputsBuffer, and eventually both sent to clients and moved into here.
        std::unordered_map<FAWorld::Tick, std::vector<FAWorld::PlayerInput>> mInputs;

        FAWorld::Tick mLastSentTick = -1;

        ENetHost* mHost = nullptr;
        ENetAddress mAddress;

        uint32_t mNextPeerId = 1;
        std::map<uint32_t, Peer> mPeers;

        Misc::Averager mStatsAverager;

        static constexpr size_t UPDATE_PACKET_START_PADDING = 50;
        static constexpr size_t MAX_UPDATE_PACKET_SIZE = 1000;
    };
}
