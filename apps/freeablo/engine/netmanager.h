#ifndef FA_NET_MANAGER_H
#define FA_NET_MANAGER_H

#include <vector>
#include <set>
#include <map>

#include <enet/enet.h>

namespace FAWorld
{
    class PlayerFactory;
    class Player;
}

namespace FARender
{
    class FASpriteGroup;
}

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
            static NetManager* get();

            NetManager(bool isServer, const FAWorld::PlayerFactory& playerFactory);
            ~NetManager();

            void update();

            FARender::FASpriteGroup* getServerSprite(size_t index);

        private:
            static constexpr uint8_t UNRELIABLE_CHANNEL_ID = 0;
            static constexpr uint8_t RELIABLE_CHANNEL_ID = 1;

            const FAWorld::PlayerFactory& mPlayerFactory;

            void sendServerPacket();
            void sendClientPacket();

            void readServerPacket(ENetEvent& event);
            void readClientPacket(ENetEvent& event);

            void sendLevel(size_t levelIndex, ENetPeer* peer);
            void readLevel(ENetPacket* packet, size_t& position);

            void sendSpriteRequest(ENetPeer* peer);
            void readSpriteRequest(ENetPacket* packet, ENetPeer* peer, size_t& position);
            void readSpriteResponse(ENetPacket* packet, size_t& position);

            FAWorld::Player* spawnPlayer(int32_t id);

            bool mIsServer;

            ENetPeer* mServerPeer = NULL;
            std::vector<ENetPeer*> mClients;
            std::map<enet_uint32, FAWorld::Player*> mServerPlayerList;
            ENetHost* mHost = NULL;
            ENetAddress mAddress;

            size_t mTick = 0;
            size_t mLastServerTickProcessed = 0;

            int32_t mLevelIndexTmp; // TODO: remove this when we fix mp level changing

            std::set<size_t> mAlreadySentServerSprites;
            std::set<size_t> mUnknownServerSprites;

            bool mClientRecievedId = false;
    };
}

#endif
