#ifndef FA_CLIENT_H

#include <stdint.h>

#include <unordered_set>

#include "enet/enet.h"

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

        private: // methods
            void updateImp(uint32_t tick);
            void receiveLevel(ENetPacket* packet, size_t position);
            void readServerPacket(ENetEvent& event, uint32_t tick);
            void sendClientPacket();
            void sendSpriteRequest();
            void readSpriteResponse(ENetPacket* packet, size_t& position);

        private: // members
            ENetPeer* mServerPeer = NULL;
            ENetHost* mHost = NULL;
            ENetAddress mAddress;

            bool mClientRecievedId = false;
            uint32_t mClientTickWhenLastServerPacketReceived = 0;
            uint32_t mLastServerTickProcessed = 0;
            uint32_t mLevelIndexTmp = 0;

            std::unordered_set<size_t> mAlreadySentServerSprites;
            std::unordered_set<size_t> mUnknownServerSprites;
    };
}

#endif