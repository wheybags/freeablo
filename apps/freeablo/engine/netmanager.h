#ifndef FA_NET_MANAGER_H
#define FA_NET_MANAGER_H

#include <enet/enet.h>

namespace Engine
{
    class NetManager
    {
        public:
            NetManager(bool isServer);
            ~NetManager();

            void update();

        private:
            void sendServerPacket();
            void sendClientPacket();

            void readServerPacket(ENetPacket* packet);
            void readClientPacket(ENetPacket* packet);

            bool mIsServer;

            ENetPeer* mPeer = NULL;
            ENetHost* mHost = NULL;
            ENetAddress mAddress;
    };
}

#endif
