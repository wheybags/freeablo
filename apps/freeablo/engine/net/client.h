#ifndef FA_CLIENT_H
#define FA_CLIENT_H

#include <stdint.h>

#include <unordered_set>

#include "enet/enet.h"

#include "netops.h"

namespace FARender
{
    class FASpriteGroup;
}

namespace Engine
{
    class Client
    {
        public:
            Client();
            ~Client();

            void update(uint32_t tick);
            FARender::FASpriteGroup* getServerSprite(size_t index);
            void sendLevelChangePacket(int32_t level);

        private: // methods
            void updateImp(uint32_t tick);
            void receiveLevel(std::shared_ptr<ReadPacket> packet);
            void readServerPacket(ENetEvent& event, uint32_t tick);
            void handleGameUpdateMessage(std::shared_ptr<ReadPacket> packet, uint32_t tick);
            void sendClientPacket();
            void sendSpriteRequest();
            Serial::Error::Error readSpriteResponse(std::shared_ptr<ReadPacket> packet);

        private: // members
            ENetPeer* mServerPeer = NULL;
            ENetHost* mHost = NULL;
            ENetAddress mAddress;

            uint32_t mClientTickWhenLastServerPacketReceived = 0;
            uint32_t mLastServerTickProcessed = 0;

            std::unordered_set<uint32_t> mAlreadySentServerSprites;
            std::unordered_set<uint32_t> mUnknownServerSprites;
    };
}

#endif
