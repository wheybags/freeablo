#pragma once
#include "multiplayerinterface.h"
#include <enet/enet.h>
#include <map>

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

        virtual std::vector<FAWorld::PlayerInput> getAndClearInputs() override;
        virtual void update() override;

    private:
        void onPeerConnect(const ENetEvent& event);
        void readPeerPacket(const ENetEvent& event);

        struct Peer
        {
            Peer() = default;
            explicit Peer(ENetPeer* peer) : peer(peer) {}

            ENetPeer* peer = nullptr;
            bool hasMap = false;
        };

        FAWorld::World& mWorld;
        LocalInputHandler& mLocalInputHandler;
        //        std::vector<FAWorld::PlayerInput> mInputs;

        ENetHost* mHost = nullptr;
        ENetAddress mAddress = {};
        std::map<uint32_t, Peer> mPeers;
    };
}
