#ifndef FA_NET_MANAGER_H
#define FA_NET_MANAGER_H

#include <vector>
#include <set>
#include <unordered_map>
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

    class Client;
    class Server;

    class NetManager
    {
        public:
            static NetManager* get();

            NetManager(bool isServer, const FAWorld::PlayerFactory& playerFactory);
            ~NetManager();

            void update();

            FARender::FASpriteGroup* getServerSprite(size_t index);

        private:

            uint32_t mTick = 0;
            bool mIsServer;
            Client* mClient;
            Server* mServer;
    };
}

#endif
