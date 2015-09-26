#ifndef FA_NET_MANAGER_H
#define FA_NET_MANAGER_H

#include <vector>

#include <enet/enet.h>

namespace Engine
{
    template <typename T>
    bool writeToPacket(ENetPacket* packet, size_t& position, const T& data)
    {
        size_t end = position + sizeof(T);

        if(end > packet->dataLength)
            return false;

        T* dest = (T*)(packet->data + position);
        *dest = data;

        position = end;

        return true;
    }

    template <typename T>
    bool readFromPacket(ENetPacket* packet, size_t& position, T& dest)
    {
        size_t end = position + sizeof(T);

        if(end > packet->dataLength)
            return false;

        T* data = (T*)(packet->data + position);
        dest = *data;

        position = end;

        return true;
    }

    class NetManager
    {
        public:
            NetManager(bool isServer);
            ~NetManager();

            void update();

        private:
            const uint32_t SERVER_PLAYER_ID = 0;
            const uint8_t UNRELIABLE_CHANNEL_ID = 0;
            const uint8_t RELIABLE_CHANNEL_ID = 1;

            void sendServerPacket();
            void sendClientPacket();

            void readServerPacket(ENetEvent& event);
            void readClientPacket(ENetEvent& event);

            void sendLevel(size_t levelIndex, ENetPeer* peer);
            void readLevel(ENetPacket* packet);

            void spawnPlayer(uint32_t id);

            bool mIsServer;

            ENetPeer* mServerPeer = NULL;
            std::vector<ENetPeer*> mClients;
            ENetHost* mHost = NULL;
            ENetAddress mAddress;

            size_t mTick = 0;
            size_t mLastServerTickProcessed = 0;

            int32_t mLevelIndexTmp; // TODO: remove this when we fix mp level changing
    };
}

#endif
