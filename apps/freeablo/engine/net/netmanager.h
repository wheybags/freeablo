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

            bool isServer()
            {
                return mIsServer;
            }

            // TODO: don't proxy these calls? Just add a getter for mClient?
            FARender::FASpriteGroup* getServerSprite(size_t index);
            void sendLevelChangePacket(int32_t level);


        private:

            uint32_t mTick = 0;
            bool mIsServer = false;
            Client* mClient = nullptr;
            Server* mServer = nullptr;
    };
}

#endif
