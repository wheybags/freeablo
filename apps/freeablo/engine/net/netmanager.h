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
