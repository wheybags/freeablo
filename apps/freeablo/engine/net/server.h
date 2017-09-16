#ifndef FA_SERVER_H
#define FA_SERVER_H

#include <unordered_map>

#include <enet/enet.h>

#include <serial/bitstream.h>

#include "../../faworld/playerfactory.h"

#include "netops.h"

namespace Engine
{
    struct ClientData
    {
        ClientData(FAWorld::Player* p) : player(p), lastReceiveTick(0) {}
        ClientData() {}

        FAWorld::Player* player;
        uint32_t lastReceiveTick;
    };

    class Server
    {
        public:
            Server(const FAWorld::PlayerFactory& playerFactory);
            ~Server();

            void update(uint32_t tick);

        private: // methods
            void updateImp(uint32_t tick);
            FAWorld::Player* spawnPlayer(int32_t id);
            void sendLevel(size_t levelIndex, ENetPeer* peer);
            void sendServerPacket(uint32_t tick);
            void readClientPacket(ENetEvent& event, uint32_t tick);
            Serial::Error::Error readClientUpdatePacket(std::shared_ptr<ReadPacket> packet, ENetPeer* peer, uint32_t tick);
            Serial::Error::Error readClientLevelChangePacket(std::shared_ptr<ReadPacket> packet, ENetPeer* peer);
            Serial::Error::Error readSpriteRequest(std::shared_ptr<ReadPacket> packet, ENetPeer* peer);
            Serial::Error::Error answerSpriteRequest(std::vector<std::string>& paths, std::vector<uint32_t>& requestedSprites, ENetPeer* peer);
            void handleNewClient(ENetPeer* peer);


        private: // members
            FAWorld::PlayerFactory mPlayerFactory;

            std::vector<ENetPeer*> mClients;
            std::unordered_map<enet_uint32, ClientData> mServersClientData;
            ENetHost* mHost = NULL;
            ENetAddress mAddress;
    };
}

#endif // !FA_SERVER_H
